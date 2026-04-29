#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

void unity_output_start();
void unity_output_char(char);

#define UNITY_OUTPUT_CHAR(a) unity_output_char(a)
#define UNITY_OUTPUT_START() unity_output_start()

// Другие макроопредления, которые можно изменить при необходимости
// #define UNITY_OUTPUT_CHAR_HEADER_DECLARATION
// #define UNITY_OUTPUT_FLUSH()
// #define UNITY_OUTPUT_FLUSH_HEADER_DECLARATION
// #define UNITY_OUTPUT_COMPLETE()

#define USE_PWR_DIRECT_SMPS_SUPPLY
#define SMPS


#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* UNITY_CONFIG_H */