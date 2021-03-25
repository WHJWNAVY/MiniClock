#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define PROJECT_NAME "miniClock"
#define IMAGE_FILE_EXT ".hex"
#define IMAGE_FILE_NAME "Objects\\" PROJECT_NAME IMAGE_FILE_EXT
#define BACKUP_FILE_NAME "back_up\\" PROJECT_NAME "_S%04uH%04u_%s_%04u%02u%02u" IMAGE_FILE_EXT
#define VERSION_DATA_FMT "S%04uH%04uC%04u"

typedef enum user_id_e
{
    USER_ID_DEMO = 0,
    USER_ID_XIUXIU,
    USER_ID_WEIXIN,
    USER_ID_YUHONGQIANG,
    USER_ID_MAX,
} user_id_e;

char* user_cfg_name[USER_ID_MAX] = {
    "WNAVY",
    "XIUBAOBAO",
    "WEIXIN",
    "YUHONGQIANG",
};

int main(void)
{
    int ret = 0;
    FILE* fq = NULL;
    time_t tm_t;
    struct tm* year = NULL;
    char output[512] = {0};
    char input[512] = {0};
    
    uint32_t ver_m = 0;
    uint32_t ver_n = 0;
    uint32_t usr_id = 0;

    tm_t = time(NULL);
    year = localtime(&tm_t);
    
    if(year == NULL)
    {
        printf("Fail to get current time!");
        ret = -1;
        goto err;
    }
    
    fq = fopen("version.dat", "r+");
    if(fq == NULL)
    {
        printf("Fail to open version data file!");
        ret = -2;
        goto err;
    }
    
    if(fgets(input, sizeof(input), fq) == NULL)
    {
        printf("Fail to gets version data from file!");
        ret = -3;
        goto err;
    }

    if(sscanf(input, VERSION_DATA_FMT, &(ver_m), &(ver_n), &(usr_id)) <= 0)
    {
        printf("Fail to sscanf version data from file!");
        ret = -4;
        goto err;
    }

    ver_m = ((ver_m > 9999) ? 0: ver_m);
    ver_n = ((ver_n > 9999) ? 0: ver_n);
    usr_id = ((usr_id >= USER_ID_MAX) ? 0: usr_id);

    sprintf(output, BACKUP_FILE_NAME,
                    ver_m, ver_n,
                    user_cfg_name[usr_id],
                    ((year->tm_year) + 1900),
                    (year->tm_mon + 1),
                    (year->tm_mday));
    #if 0
    ret = rename(IMAGE_FILE_NAME, output);
    if(ret != 0)
    {
        printf("Fail to rename [%s] to [%s], err[%d]!", IMAGE_FILE_NAME, output, ret);
        ret = -5;
        goto err;
    }
    #else
    memset(input, 0, sizeof(input));
    sprintf(input, "copy /y %s %s", IMAGE_FILE_NAME, output);
    printf("run cmd [%s]\n", input);
    system(input);
    #endif
    printf("Image file[%s], Backup file[%s].", IMAGE_FILE_NAME, output);

    ret = 0;
err:

    if(fq != NULL)
    {
        fclose(fq);
    }
    return ret;
}