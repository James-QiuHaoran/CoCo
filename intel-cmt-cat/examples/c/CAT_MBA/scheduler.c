/**
 * CoCo Scheduler
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pqos.h"

#define PQOS_MAX_CORES (1024)
#define NUM_APPS (3)
#define NUM_CLOSS (3)

/**
 * A struct type recoding the core-to-CLOS association
 */
typedef struct Association {
	unsigned num_cores;
	unsigned *core_list;
	unsigned class_id;
} Association;

/**
 * Number of cores selected for cache allocation association
 */
static int sel_l3ca_assoc_num = 0;

/**
 * Maintains a table of core and class_id that are selected in config string for
 * setting up allocation policy per core
 */
static struct {
        unsigned core;
        unsigned class_id;
} sel_l3ca_assoc_tab[PQOS_MAX_CORES];

/**
 * @brief Prints information about cache allocation settings in the system
 */
static void
print_allocation_config(void)
{
        int ret;
        unsigned i;
        unsigned l3cat_id_count, *l3cat_ids = NULL;
        const struct pqos_cpuinfo *p_cpu = NULL;
        const struct pqos_cap *p_cap = NULL;

        /* Get CMT capability and CPU info pointer */
        ret = pqos_cap_get(&p_cap, &p_cpu);
        if (ret != PQOS_RETVAL_OK) {
                printf("Error retrieving PQoS capabilities!\n");
                return;
        }
        /* Get CPU l3cat_id information to set COS */
        l3cat_ids = pqos_cpu_get_l3cat_ids(p_cpu, &l3cat_id_count);
        if (l3cat_ids == NULL) {
                printf("Error retrieving CPU socket information!\n");
                return;
        }
        for (i = 0; i < l3cat_id_count; i++) {
                unsigned *lcores = NULL;
                unsigned lcount = 0, n = 0;

                lcores = pqos_cpu_get_cores(p_cpu, l3cat_ids[i], &lcount);
                if (lcores == NULL || lcount == 0) {
                        printf("Error retrieving core information!\n");
                        free(lcores);
                        free(l3cat_ids);
                        return;
                }
                printf("Core information for socket %u:\n", l3cat_ids[i]);
                for (n = 0; n < lcount; n++) {
                        unsigned class_id = 0;

                        ret = pqos_alloc_assoc_get(lcores[n], &class_id);
                        if (ret == PQOS_RETVAL_OK)
                                printf("    Core %u => COS%u\n", lcores[n],
                                       class_id);
                        else
                                printf("    Core %u => ERROR\n", lcores[n]);
                }
                free(lcores);
        }
        free(l3cat_ids);
}
/**
 * @brief Sets up association between cores and allocation classes of service
 *
 * @return Number of associations made
 * @retval 0 no association made (nor requested)
 * @retval negative error
 * @retval positive success
 */
static int
set_allocation_assoc(void)
{
        int i;

        for (i = 0; i < sel_l3ca_assoc_num; i++) {
                int ret;

                ret = pqos_alloc_assoc_set(sel_l3ca_assoc_tab[i].core,
                                           sel_l3ca_assoc_tab[i].class_id);
                if (ret != PQOS_RETVAL_OK) {
                        printf("Setting allocation class of service "
                               "association failed!\n");
                        return -1;
                }
        }
        return sel_l3ca_assoc_num;
}

static int
set_allocation_assoc_with_args(int sel_l3ca_assoc_num, unsigned class_id, unsigned *core_list)
{
	for (int i = 0; i < sel_l3ca_assoc_num; i++) {
		int ret = pqos_alloc_assoc_set(core_list[i], class_id);
		if (ret != PQOS_RETVAL_OK) {
			printf("Setting allocation class of service "
                               "association failed!\n");
                        return -1;
		}
	}
	return PQOS_RETVAL_OK;
}

int main(void) {
        struct pqos_config cfg;
        int ret, exit_val = EXIT_SUCCESS;

        memset(&cfg, 0, sizeof(cfg));
        cfg.fd_log = STDOUT_FILENO;
        cfg.verbose = 0;
        /* PQoS Initialization - Check and initialize CAT and CMT capability */
        ret = pqos_init(&cfg);
        if (ret != PQOS_RETVAL_OK) {
                printf("Error initializing PQoS library!\n");
                exit_val = EXIT_FAILURE;
                goto error_exit;
        }

        /* Get input from user */
        // enforcement_get_input(argc, argv);
	
	int num_cores_list[NUM_APPS] = { 4, 4, 4 };
	int cores_list[NUM_APPS][4] = {
		{ 11, 12, 13, 14 },
		{ 21, 22, 23, 24 },
		{ 31, 32, 33, 34 }
		// { 41, 42, 43, 44 },
        	// { 51, 52, 53, 54 },
        	// { 61, 62, 63, 64 }
	};
	int weights_list[NUM_APPS][NUM_CLOSS] = {
		{ 3, 2, 1 },
		{ 1, 3, 2 },
		{ 2, 1, 3 }
	};
	int working_set[NUM_CLOSS] = { 0, 1, 2 };

	Association cores_to_CLOS_mapping[NUM_APPS];

	// Init
	for (int i = 0; i < NUM_APPS; i++) {
		cores_to_CLOS_mapping[i].num_cores = num_cores_list[i];
		// clos0 is the common pool
		// clos1 clos2 clos3 are clos to be scheduled on
		cores_to_CLOS_mapping[i].class_id = i + 1;
		working_set[i] = i;
		cores_to_CLOS_mapping[i].core_list = malloc(num_cores_list[i] * sizeof(unsigned));
		for (int j = 0; j < num_cores_list[i]; j++) {
			cores_to_CLOS_mapping[i].core_list[j] = cores_list[i][j];
		}
	}

	// start round-robin
	// int round = 10;
	while (1) {
	// while (round > 0) {
		// round -= 1;
		// perform config to cat
		for (int i = 0; i < NUM_APPS; i++) {
			int ret = set_allocation_assoc_with_args(num_cores_list[i], cores_to_CLOS_mapping[i].class_id, cores_to_CLOS_mapping[i].core_list);
			if (ret != PQOS_RETVAL_OK) {
				printf("CAT association error!\n");
                        	goto error_exit;
			}
		}
		printf("Allocation configuration altered.\n");

		// print current config
		for (int i = 0; i < NUM_APPS; i++) {
			printf("App #%d | ", i);
			printf("CLOS #%d (Weight = %d) | ", cores_to_CLOS_mapping[i].class_id, weights_list[i][cores_to_CLOS_mapping[i].class_id-1]);
			printf("Cores: ");
			for (int j = 0; j < num_cores_list[i]; j++) {
				printf("%d ", cores_to_CLOS_mapping[i].core_list[j]);
			}
			printf("\n");
		}
		printf("Working set: | ");
		for (int i = 0; i < NUM_CLOSS; i++) {
			printf("App #%d | ", working_set[i]);
		}
		printf("\n");
		print_allocation_config();
		printf("===== End Round =====\n");

		// wait for weighted time slice
		int time_to_sleep = weights_list[working_set[0]][0];
		sleep(time_to_sleep);

		// set up the new config
		for (int i = 0; i < NUM_APPS; i++) {
			cores_to_CLOS_mapping[i].class_id += 1;
			// class id starting from 1
			if (cores_to_CLOS_mapping[i].class_id > 3) {
				cores_to_CLOS_mapping[i].class_id = 1;
			}
                	working_set[cores_to_CLOS_mapping[i].class_id - 1] = i;
		}
	}

error_exit:
        /* reset and deallocate all the resources */
        ret = pqos_fini();
        if (ret != PQOS_RETVAL_OK)
                printf("Error shutting down PQoS library!\n");
        return exit_val;
}
