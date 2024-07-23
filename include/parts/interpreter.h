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

    virtual void predict(const cv::Mat &input) = 0;

    virtual void load(const std::string model_path) = 0;
};

class TfLiteInterpreter : public Interpreter {
   public:
    TfLiteInterpreter();

    virtual void predict(const cv::Mat &input);

    /**
     * @brief Load and initialize the TFLite model
     * TODO: figure out how best to make this publicly accessible/callable
     *
     * @param model_path
     */
    virtual void load(const std::string model_path);

   private:
    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
    std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver> m_resolver;
    int m_input_batch_size = 0;
    int m_input_height = 0;
    int m_input_width = 0;
    int m_input_channels = 0;

    // TfLiteDelegate* delegate_;
    std::string m_model_file;
    int m_num_threads;
};
}  // namespace donkeycar::vision