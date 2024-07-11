#pragma once

#include "parts/image.h"
#include "parts/part-io.h"
#include "parts/part.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/string_util.h"

#define TFLITE_MINIMAL_CHECK(x)                                  \
    if (!(x)) {                                                  \
        fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
        exit(1);                                                 \
    }

namespace donkeycar::vision {

class Interpreter {
    // ABC to delegate between Keras and TFLite
   public:
    Interpreter() {}

    // TODO
    virtual void load(const std::string model_path) = 0;
    virtual void predict() = 0;
};

class TfLiteInterpreter : public Interpreter {
   public:
    TfLiteInterpreter() {}

    /**
     * @brief Load and initialize the TFLite model
     *
     * @param model_path
     */
    virtual void load(const std::string model_path) {
        // Load model
        this->m_model =
            tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
        TFLITE_MINIMAL_CHECK(m_model);

        // Build the interpreter
        this->m_resolver =
            std::make_unique<tflite::ops::builtin::BuiltinOpResolver>();
        this->m_interpreter = std::make_unique<tflite::Interpreter>();
        tflite::InterpreterBuilder builder(*m_model, *this->m_resolver);
        builder(&this->m_interpreter);
        TFLITE_MINIMAL_CHECK(this->m_interpreter != nullptr);
        TFLITE_MINIMAL_CHECK(this->m_interpreter->AllocateTensors() ==
                             kTfLiteOk);

        this->m_interpreter->SetAllowFp16PrecisionForFp32(true);

        /*
        this->m_interpreter->SetNumThreads(m_num_threads);
        if (status != TfLiteStatus::kTfLiteOk) {
            std::cerr << "Failed to SetNumThreads to " << num_threads_
                      << std::endl;
            // return false;
        }
        */

#ifdef TFLITE_ENABLE_GPU
        // GPU Delegate
        auto gpu_options = TfLiteGpuDelegateOptionsV2Default();
        gpu_options.inference_preference =
            TFLITE_GPU_INFERENCE_PREFERENCE_SUSTAINED_SPEED;
        gpu_options.inference_priority1 =
            TFLITE_GPU_INFERENCE_PRIORITY_MIN_LATENCY;
        this->delegate_ = TfLiteGpuDelegateV2Create(&gpu_options);
        status = this->interpreter_->ModifyGraphWithDelegate(this->delegate_);
        if (status != TfLiteStatus::kTfLiteOk) {
            std::cerr << "Failed to ModifyGraphWithDelegate." << std::endl;
        }
#endif
    }

    virtual void predict() { /*printf("[TFLiteInterpreter] model predict!\n");*/
    }

   private:
    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
    std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver> m_resolver;

    // TfLiteDelegate* delegate_;
    std::string m_model_file;
    int m_num_threads;
    // int input_tensor_width_;
    // int input_tensor_height_;
    // float confidence_thresh_;
};  // namespace donkeycar::vision
}  // namespace donkeycar::vision