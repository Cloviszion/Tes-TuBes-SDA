#ifndef CJSON_H
#define CJSON_H

/* cJSON Types */
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)

/* cJSON structure */
typedef struct cJSON {
    struct cJSON *next, *prev; /* For linked list in arrays/objects */
    struct cJSON *child;       /* Children for arrays/objects */
    int type;                  /* Type of the item (cJSON_False, etc.) */
    char *valuestring;         /* String value */
    double valuedouble;        /* Number value */
    char *string;              /* Name of the item (key in objects) */
} cJSON;

/* Function prototypes */
cJSON *cJSON_Parse(const char *value);
void cJSON_Delete(cJSON *item);
cJSON *cJSON_CreateObject(void);
cJSON *cJSON_CreateArray(void);
void cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
void cJSON_AddItemToArray(cJSON *array, cJSON *item);
cJSON *cJSON_CreateString(const char *string);
char *cJSON_Print(cJSON *item);
cJSON *cJSON_GetObjectItem(cJSON *object, const char *string);
int cJSON_GetArraySize(cJSON *array);
cJSON *cJSON_GetArrayItem(cJSON *array, int index);
int cJSON_IsArray(cJSON *item);

#endif