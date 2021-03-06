#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

#include "../include/fnclib.h"
#include "../include/defines.h"
#include "../include/diseaseMonitor.h"

int DM_Init(const char* fileName, ListPtr list, HashTablePtr h1, HashTablePtr h2)
{
    char *dest = NULL;
    FILE *filePtr = NULL;
    ListNodePtr node = NULL;
    PatientPtr patient = NULL;

    if ((dest = malloc(strlen(FILE_LOCATION) + strlen(fileName) + 1)) == NULL) {
        perror("malloc failed");
        return -1;
    }
    strcpy(dest, FILE_LOCATION);
    strcat(dest, fileName);

    if ((filePtr = fopen(dest, "r")) == NULL) {
        perror(dest);
        free(dest);
        return -1;
    }

    while ((patient = DM_GetPatient(filePtr)) != NULL)
    {
        if (!DM_ValidatePatient(list, patient)) {
            Patient_Print(patient);

            if ((node = List_InsertSorted(list, patient)) == NULL) {
                return -1;
            }

            if (HashTable_Insert(h1, patient->diseaseID, node) == -1) {
                return -1;
            }

            if (HashTable_Insert(h2, patient->country, node) == -1) {
                return -1;
            }
        }
        else {
            Patient_Close(patient);
        }
    }
    printf("%d\n", list->len);
    printf("\n");

    free(dest);
    fclose(filePtr);

    return 0;
}

int DM_Run(char* line, ListPtr list, HashTablePtr h1, HashTablePtr h2)
{
    wordexp_t p;
    DatePtr d1 = NULL, d2 = NULL;

    wordexp(line, &p, 0);

    if (!strcmp(p.we_wordv[0], "/globalDiseaseStats"))
    {
        if (p.we_wordc == 1 || p.we_wordc == 3) {

            if (p.we_wordc == 3) {
                if((d1 = Date_Init(p.we_wordv[1])) == NULL || (d2 = Date_Init(p.we_wordv[2])) == NULL) {
                    return -1;
                }
            }

            globalDiseaseStats(h1, d1, d2);

            if(d1 != NULL && d2 != NULL) {
                free(d1);
                free(d2);
            }
        }
        else {
            printf("You need to include both entry and exit dates\n");
        }
    }
    else if (!strcmp(p.we_wordv[0], "/diseaseFrequency"))
    {
        if (p.we_wordc == 4 || p.we_wordc == 5) {
            if((d1 = Date_Init(p.we_wordv[2])) == NULL || (d2 = Date_Init(p.we_wordv[3])) == NULL) {
                return -1;
            }

            diseaseFrequency(h1, p.we_wordv[1], p.we_wordv[4], d1, d2);

            free(d1);
            free(d2);
        }
        else {
            printf("Wrong input\n");
        }
    }
    else if (!strcmp(p.we_wordv[0], "/topk-Diseases"))
    {
        if (p.we_wordc == 3 || p.we_wordc == 5) {

            if (p.we_wordc == 5) {
                if((d1 = Date_Init(p.we_wordv[3])) == NULL || (d2 = Date_Init(p.we_wordv[4])) == NULL) {
                    return -1;
                }
            }

            topk_Diseases(h1, h2, p.we_wordv[2], atoi(p.we_wordv[1]), d1, d2);
        }
        else {
            printf("You need to include both entry and exit dates\n");
        }
    }
    else if (!strcmp(p.we_wordv[0], "/topk-Countries"))
    {
        if (p.we_wordc == 3 || p.we_wordc == 5) {

            if (p.we_wordc == 5) {
                if((d1 = Date_Init(p.we_wordv[3])) == NULL || (d2 = Date_Init(p.we_wordv[4])) == NULL) {
                    return -1;
                }
            }

            topk_Countries(h1, h2, p.we_wordv[2], atoi(p.we_wordv[1]), d1, d2);
        }
        else {
            printf("You need to include both entry and exit dates\n");
        }
    }
    else if (!strcmp(p.we_wordv[0], "/insertPatientRecord"))
    {
        if ( p.we_wordc == 7 || p.we_wordc == 8) {
            insertPatientRecord(list, h1, h2, p.we_wordv[1], p.we_wordv[2], p.we_wordv[3], p.we_wordv[4], p.we_wordv[5], p.we_wordv[6], p.we_wordv[7]);
        }
        else {
            printf("Wrong input\n");
        }
    }
    else if (!strcmp(p.we_wordv[0], "/recordPatientExit"))
    {
        if (p.we_wordc == 3) {
            recordPatientExit(list, p.we_wordv[1], p.we_wordv[2]);
        }
        else {
            printf("Wrong input\n");
        }
    }
    else if (!strcmp(p.we_wordv[0], "/numCurrentPatients"))
    {
        if (p.we_wordc == 1 || p.we_wordc == 2) {
            numCurrentPatients(h1, p.we_wordv[1]);
        }
        else {
            printf("Wrong input\n");
        }
    }
    else {
        printf("Unknown command: %s\n", p.we_wordv[0]);
    }
    printf("\n");

    wordfree(&p);

    return 0;
}

PatientPtr DM_GetPatient(FILE *filePtr)
{
    char *line = NULL;
    size_t len = 0;
    PatientPtr patient = NULL;

    if(getline(&line, &len, filePtr) == -1) {
        free(line);
        return NULL;
    }

    if((patient = Patient_Init(line)) == NULL) {
        free(line);
        return NULL;
    }

    free(line);

    return patient;
}

int DM_PatientExist(const ListPtr list, const PatientPtr patient)
{
    ListNodePtr node = list->head;

    // bariemai na ftiaksw hash table sorry 

    while (node != NULL)
    {
        if (!strcmp(node->patient->id, patient->id))
        {
            return -1;
        }
        node = node->next;
    }

    return 0;
}

int DM_ValidatePatient(const ListPtr list, const PatientPtr patient)
{
    if (DM_PatientExist(list, patient)) {
        return -1;
    }
    else if (patient->exitDate != NULL) {
        if (Date_Compare(patient->exitDate, patient->entryDate) == -1) {
            return -1;
        }
    }

    return 0;
}