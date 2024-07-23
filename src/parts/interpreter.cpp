#include "parts/interpreter.h"
#include <iostream>

namespace donkeycar::vision {

TfLiteInterpreter::TfLiteInterpreter() : Interpreter() {}

void TfLiteInterpreter::load(const std::string model_path) {
    // Load model
    this->m_model = tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
    TFLITE_MINIMAL_CHECK(m_model);

    // Build the interpreter
    this->m_resolver =
        std::make_unique<tflite::ops::builtin::BuiltinOpResolver>();
    this->m_interpreter = std::make_unique<tflite::Interpreter>();
    tflite::InterpreterBuilder builder(*m_model, *this->m_resolver);
    builder(&this->m_interpreter);
    TFLITE_MINIMAL_CHECK(this->m_interpreter != nullptr);
    TFLITE_MINIMAL_CHECK(this->m_interpreter->AllocateTensors() == kTfLiteOk);

    this->m_interpreter->SetAllowFp16PrecisionForFp32(true);

    // Obtain a pointer to the model's one and only input tensor
    const std::vector<int>& t_inputs = m_interpreter->inputs();
    TfLiteTensor* input = m_interpreter->tensor(t_inputs[0]);
    m_input_batch_size = input->dims->data[0];
    m_input_height = input->dims->data[1];
    m_input_width = input->dims->data[2];
    m_input_channels = input->dims->data[3];
    printf("[tensor input] batch_size: %d, h: %d, w: %d, channels: %d\n",
           m_input_batch_size, m_input_height, m_input_width, m_input_channels);

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
    gpu_options.inference_priority1 = TFLITE_GPU_INFERENCE_PRIORITY_MIN_LATENCY;
    this->delegate_ = TfLiteGpuDelegateV2Create(&gpu_options);
    status = this->interpreter_->ModifyGraphWithDelegate(this->delegate_);
    if (status != TfLiteStatus::kTfLiteOk) {
        std::cerr << "Failed to ModifyGraphWithDelegate." << std::endl;
    }
#endif
}
void TfLiteInterpreter::predict(const cv::Mat& input) {
    // input detail name:  img_in
    // input detail shape: [  1 120 160   3]
    // if (input->frame().empty()) {
    if (input.empty()) {
        return;
    }

    // copy image to input as input tensor
    // cv::Mat image = input->frame();
    cv::Mat resized;
    cv::resize(input, resized, cv::Size(m_input_width, m_input_height));
    int rows = resized.rows;
    int cols = resized.cols;
    printf("[Interpreter] resized: %d x %d\n", rows, cols);
    // TODO:
    // std::memcpy(m_interpreter->typed_input_tensor<uchar>(0), resized.data,
    //            resized.total() * resized.elemSize());

    /*
    // std::cout << "tensors size: " << interpreter_->tensors_size() <<
    // "\n"; std::cout << "nodes size: " << interpreter_->nodes_size() <<
    // "\n"; std::cout << "inputs: " << interpreter_->inputs().size() <<
    // "\n"; std::cout << "input(0) name: " << interpreter_->GetInputName(0)
    // << "\n"; std::cout << "outputs: " << interpreter_->outputs().size()
    // <<
    // "\n";

    // Infer!
    auto now = std::chrono::system_clock::now();
    interpreter_->Invoke();
    auto end = std::chrono::system_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - now);
    // std::cout << "Inference: " << (1000.0f / elapsed.count()) << " fps"
    // << std::endl;
    */
}

}  // namespace donkeycar::vision