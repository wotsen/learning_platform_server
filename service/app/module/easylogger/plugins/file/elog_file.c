/*
 * @Date: 2019-08-10 22:55:30
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-11 00:20:29
 */
/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015-2019, Qintl, <qintl_linux@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Save log to file.
 * Created on: 2019-01-05
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "elog_file.h"
#include "elog_file_cfg.h"

#define ELOG_FILE_SEM_KEY   ((key_t)0x19910612)
#ifdef _SEM_SEMUN_UNDEFINED
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};
#endif

/* initialize OK flag */
static bool init_ok = false;
static FILE *fp = NULL;
static int fd = -1;
static ElogFileCfg local_cfg;

static int semid = -1;
static struct sembuf const up = {0, 1, SEM_UNDO};
static struct sembuf const down = {0, -1, SEM_UNDO};

static void lock_init(void);
static int lock_open(void);

/**
 * EasyLogger flile log pulgin port initialize
 *
 * @return result
 */
ElogErrCode elog_file_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    lock_init();

    return result;
}

/**
 * file log lock
 */
void inline elog_file_port_lock(void)
{
    semid == -1 ? -1 : semop(semid, (struct sembuf *)&down, 1);
}

/**
 * file log unlock
 */
void inline elog_file_port_unlock(void)
{
    semid == -1 ? -1 : semop(semid, (struct sembuf *)&up, 1);
}
/**
 * file log deinit
 */
void elog_file_port_deinit(void)
{

}

/**
 * initialize the lock 
 */
static void lock_init(void)
{
    int id, rc;
    union semun arg;
    struct sembuf sembuf;

    id = semget(ELOG_FILE_SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666);
    if(likely(id == -1)) {
        id = lock_open();
        if (id == -1)
            goto __exit;
    } else {
        arg.val = 0;
        rc = semctl(id, 0, SETVAL, arg);
        if (rc == -1)
            goto __exit;

        sembuf.sem_num = 0;
        sembuf.sem_op = 1;
        sembuf.sem_flg = 0;

        rc = semop(id, &sembuf, 1);
        if (rc == -1)
            goto __exit;
    }

    semid = id;
__exit:
    return ;
}

/**
 * gets the lock 
 */
static int lock_open(void)
{
    int id, rc, i;
    union semun arg;
    struct semid_ds ds;

    id = semget(ELOG_FILE_SEM_KEY, 1, 0666);
    if(unlikely(id == -1))
        goto err;

    arg.buf = &ds;

    for (i = 0; i < 10; i++) {
        rc = semctl(id, 0, IPC_STAT, arg);
        if (unlikely(rc == -1))
            goto err;

        if(ds.sem_otime != 0)
            break;

        usleep(10 * 1000);
    }

    if (unlikely(ds.sem_otime == 0))
        goto err;

    return id;
err:
    return -1;
}

ElogErrCode elog_file_init(void)
{
    ElogErrCode result = ELOG_NO_ERR;
    ElogFileCfg cfg;

    if (init_ok)
        goto __exit;

    elog_file_port_init();

    cfg.name = (char *)ELOG_FILE_NAME;
    cfg.max_size = ELOG_FILE_MAX_SIZE;
    cfg.max_rotate = ELOG_FILE_MAX_ROTATE;

    elog_file_config(&cfg);

    init_ok = true;
__exit:
    return result;
}

/*
 * rotate the log file xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0
 *
 * it will return true when rotate successfully
 */
static bool elog_file_rotate(void)
{
#define SUFFIX_LEN                     10
    /* mv xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0 */
    size_t base = strlen(local_cfg.name);
    char *oldpath = NULL, *newpath = NULL;
    int n;
    FILE *fp_bak = fp;

    oldpath = (char *) malloc(base + SUFFIX_LEN);
    newpath = (char *) malloc(base + SUFFIX_LEN);

    if (oldpath == NULL || newpath == NULL) {
        return false;
    }

    memcpy(oldpath, local_cfg.name, base);
    memcpy(newpath, local_cfg.name, base);

    for (n = local_cfg.max_rotate - 1; n >= 0; --n) {
        snprintf(oldpath + base, SUFFIX_LEN, n ? ".%d" : "", n - 1);
        snprintf(newpath + base, SUFFIX_LEN, ".%d", n);
        rename(oldpath, newpath);
    }
    free(oldpath);
    free(newpath);

    fp = fopen(local_cfg.name, "a+");
    if (fp) {
        if (fp_bak) {
            fclose(fp_bak);
        }
        fd = fileno(fp);
        return true;
    } else {
        fp = fp_bak;
        fd = -1;
        return false;
    }
}

void elog_file_write(const char *log, size_t size)
{
    ELOG_ASSERT(init_ok);
    ELOG_ASSERT(log);
    struct stat statbuf;

    statbuf.st_size = 0;

    elog_file_port_lock();

    fstat(fd, &statbuf);

    if (unlikely(statbuf.st_size > (int)(local_cfg.max_size))) {
        /* rotate the log file */
        if (local_cfg.max_rotate <= 0 || !elog_file_rotate()) {
            /* not enabled rotate or rotate failed */
            elog_file_port_unlock();
            return;
        }
    }

    fwrite(log, size, 1, fp);

#ifdef ELOG_FILE_FLUSH_CAHCE_ENABLE
    fflush(fp);
    fsync(fd);
#endif

    elog_file_port_unlock();
}

void elog_file_deinit(void)
{
    ELOG_ASSERT(init_ok);

    elog_file_port_deinit();
    fclose(fp);
}

void elog_file_config(ElogFileCfg *cfg)
{
    if (fp) {
        fclose(fp);
    }

    elog_file_port_lock();

    local_cfg.name = cfg->name;
    local_cfg.max_size = cfg->max_size;
    local_cfg.max_rotate = cfg->max_rotate;

    fp = fopen(local_cfg.name, "a+");
    if (fp)
        fd = fileno(fp);
    else
        fd = -1;

    elog_file_port_unlock();
}
