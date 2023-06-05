#include <iostream>
#include "header/ModelBase.hpp"
#include "header/VitsSvc.hpp"
#include <chrono>

int main() {
    rapidjson::Document Config;
    Config.Parse(R"({
    "Folder" : "march7",
    "Name" : "march7",
    "Type" : "RVC",
    "Rate" : 48000,
    "Hop" : 320,
    "Cleaner" : "",
    "Hubert": "hubert4.0",
    "Diffusion": false,
    "CharaMix": false,
    "Volume": false,
    "HiddenSize": 256,
    "Characters" : ["march7"]
	})");

    //Progress bar
    const InferClass::BaseModelType::callback ProgressCallback = [](size_t a, size_t b) {std::cout << std::to_string((float)a * 100.f / (float)b) << "%\n"; };

    //return params for inference
    const InferClass::BaseModelType::callback_params ParamsCallback = []()
    {
        auto params = InferClass::InferConfigs();
        params.kmeans_rate = 0.5;
        params.keys = 8;
        return params;
    };

    //modify duration per phoneme
    InferClass::TTS::DurationCallback DurationCallback = [](std::vector<float>&) {};

    std::vector<int16_t> output;
    try
    {
        std::wstring inp;
        const auto model = dynamic_cast<InferClass::BaseModelType*>(
            new InferClass::VitsSvc(Config, ProgressCallback, ParamsCallback)
            );

        do
        {
            std::wstring input;

            std::wcout << "输入源音频路径：" << std::endl;
            std::wcin >> input;
            // 在这里可以根据输入指令进行相应的处理
            std::wcout << "输入指令为：" << input << std::endl;


            // 记录开始时间
            auto start = std::chrono::high_resolution_clock::now();
            output = model->Inference(input);


            std::wstring outpath = GetCurrentFolder() + L"\\test.wav";
            std::cout << outpath.c_str();
            const Wav outWav(model->GetSamplingRate(), output.size() * 2, output.data());
            outWav.Writef(outpath);

            // 记录结束时间
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间
            std::chrono::duration<double, std::milli> elapsed = end - start;

            // 输出执行时间
            std::cout << "代码执行时间: " << elapsed.count() << " ms" << std::endl;

        } while (true);

        delete model;
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }
}