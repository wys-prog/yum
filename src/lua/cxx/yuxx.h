#ifndef ___YUM_CXXX_H___
#define ___YUM_CXXX_H___

  #ifdef __cplusplus
  extern "C" {
  #endif // __cplusplus

    int32_t     YcxxYum_initSubsystem       (void);
    void        YcxxYum_shutdownSubsystem   (void);
    void        YcxxYum_pushInt             (int64_t a);
    void        YcxxYum_pushNumber          (double a);
    void        YcxxYum_pushString          (const char *a);
    void        YcxxYum_pushBoolean         (int16_t a);
    int32_t     YcxxYum_loadString          (const char *str);
    int32_t     YcxxYum_loadFile            (const char *src);
    int32_t     YcxxYum_call                (const char *name);

  #ifdef __cplusplus
  } /* extern "C" */
  #endif // __cplusplus

#endif // ___YUM_CXXX_H___