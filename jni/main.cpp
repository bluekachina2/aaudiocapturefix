#include <android/log.h>
#include <string.h>
#include "zygisk.hpp"

#define LOG_TAG "AAudioCaptureFix"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static const char *TARGET_PACKAGE = "com.spotify.music";

class AAudioCaptureFixModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        api_ = api;
        env_ = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        const char *nice_name = nullptr;

        if (args != nullptr && args->nice_name != nullptr) {
            nice_name = env_->GetStringUTFChars(args->nice_name, nullptr);
        }

        is_target_ = (
            nice_name != nullptr &&
            strcmp(nice_name, TARGET_PACKAGE) == 0
        );

        if (nice_name != nullptr) {
            env_->ReleaseStringUTFChars(args->nice_name, nice_name);
        }

        if (!is_target_) {
            api_->setOption(zygisk::DLCLOSE_MODULE_LIBRARY);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        if (!is_target_) {
            return;
        }

        LOGI("Diagnostic module loaded into Spotify successfully.");
        LOGI("AAudio hook is disabled for this test.");
    }

private:
    zygisk::Api *api_ = nullptr;
    JNIEnv *env_ = nullptr;
    bool is_target_ = false;
};

REGISTER_ZYGISK_MODULE(AAudioCaptureFixModule)
