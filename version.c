#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define VERSION_AUTO_INCREASE 0
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
    "CONFIG_PRODUCT_WNAVY",
    "CONFIG_PRODUCT_XIUBAOBAO",
    "CONFIG_PRODUCT_WEIXIN",
    "CONFIG_PRODUCT_YUHONGQIANG",
};

int main(void)
{
    int ret = 0;
    FILE* fp = NULL;
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
    
    fp = fopen("src\\version.h", "w+");
    if(fp == NULL)
    {
        printf("Fail to open version header file!");
        ret = -2;
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
    #if VERSION_AUTO_INCREASE
    ver_m++;
    #endif
    ver_m = ((ver_m > 9999) ? 0: ver_m);
    ver_n = ((ver_n > 9999) ? 0: ver_n);
    usr_id = ((usr_id >= USER_ID_MAX) ? 0: usr_id);
    
    sprintf(output, "#ifndef __VERSION_H__\n"
                    "#define __VERSION_H__\n"
                    "#define SYSTEM_SOFTWARE_VERSION \"V:%04u\"\n"
                    "#define SYSTEM_HARDWARE_VERSION \"V:%04u\"\n"
                    "#define SYSTEM_RELVERSION_TIME \"%02u%02u%02u\"\n"
                    "#define %s 1\n"
                    "#endif\n",
                    ver_m, ver_n,
                    ((year->tm_year) + 1900 - 2000),
                    (year->tm_mon + 1),
                    (year->tm_mday),
                    user_cfg_name[usr_id]);
    
    printf("%s", output);
    fprintf(fp, "%s", output);

    #if VERSION_AUTO_INCREASE
    fprintf(fq, VERSION_DATA_FMT, ver_m, ver_n, usr_id);
    #endif

    ret = 0;
err:
    if(fp != NULL)
    {
        fclose(fp);
    }

    if(fq != NULL)
    {
        fclose(fq);
    }
    return ret;
}