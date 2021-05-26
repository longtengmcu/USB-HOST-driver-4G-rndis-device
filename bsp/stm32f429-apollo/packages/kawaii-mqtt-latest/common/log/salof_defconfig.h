/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-25 23:56:34
 * @LastEditTime: 2020-03-02 01:32:32
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _SALOF_DEFCONFIG_H_
#define _SALOF_DEFCONFIG_H_

#include "salof_config.h"

#ifdef SALOF_USING_LOG

#include <rtthread.h>
#include <rthw.h>
#include <stdio.h>

#define salof_mutex                     rt_mutex_t
#define salof_sem                       rt_sem_t
#define salof_tcb                       rt_thread_t
#define SALOF_TASK_PRIO                 (RT_THREAD_PRIORITY_MAX - 1)
    

void *salof_alloc(unsigned int size);
void salof_free(void *mem);
salof_tcb salof_task_create(const char *name,
                            void (*task_entry)(void *param),
                            void * const param,
                            unsigned int stack_size,
                            unsigned int priority,
                            unsigned int tick);
salof_mutex salof_mutex_create(void);
void salof_mutex_delete(salof_mutex mutex);
int salof_mutex_pend(salof_mutex mutex, unsigned int timeout);
int salof_mutex_post(salof_mutex mutex);
salof_sem salof_sem_create(void);
void salof_sem_delete(salof_sem sem);
int salof_sem_pend(salof_sem sem, unsigned int timeout);
int salof_sem_post(salof_sem sem);
unsigned int salof_get_tick(void);
char *salof_get_task_name(void);
extern int send_buff(char *buf, int len);

#endif

#endif // !_SALOF_DEFCONFIG_H_

