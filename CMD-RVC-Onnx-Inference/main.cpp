#include <iostream>
#include "header/ModelBase.hpp"
#include "header/VitsSvc.hpp"
#include <chrono>

rapidjson::Document parseJSONFile(const std::string& filename) {
    rapidjson::Document document;

    // ��JSON�ļ�
    std::ifstream file(filename);

    if (file.is_open()) {
        // ���ļ����ݶ����ַ���
        std::string jsonContent((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        // ����JSON�ַ���
        document.Parse(jsonContent.c_str());

        // �������Ƿ�ɹ�
        if (document.HasParseError()) {
            std::cout << "Failed to parse JSON." << std::endl;
        }

        // �ر��ļ�
        file.close();
    }
    else {
        //std::cout << "Failed to open JSON file." << std::endl;
        throw std::exception("Failed to open JSON file.\n");
    }

    return document;
}

InferClass::BaseModelType* CreateModel() {
    std::string modelPath;
    std::cout << "����ONNXģ��·����" << std::endl;
    std::cin >> modelPath;

    std::string configPath;
    std::cout << "��������·����" << std::endl;
    std::cin >> configPath;

    rapidjson::Document Config = parseJSONFile(configPath);
    // ����ַ����ֶε�Document����
    rapidjson::Document::AllocatorType& allocator = Config.GetAllocator();
    rapidjson::Value key("ModelPath", allocator);
    rapidjson::Value value(modelPath.c_str(), allocator);
    Config.AddMember(key, value, allocator);

    //Progress bar
    const InferClass::BaseModelType::callback ProgressCallback = [](size_t a, size_t b) {std::cout << std::to_string((float)a * 100.f / (float)b) << "%\n"; };

    //return params for inference
    const InferClass::BaseModelType::callback_params ParamsCallback = []()
    {
        auto params = InferClass::InferConfigs();
        params.kmeans_rate = 0.5;
        params.keys = 0;
        return params;
    };

    const auto model = dynamic_cast<InferClass::BaseModelType*>(
        new InferClass::VitsSvc(Config, ProgressCallback, ParamsCallback)
        );
    return model;
}

InferClass::BaseModelType* TryCreateModel() {

    try
    {
        return CreateModel();
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
        return TryCreateModel();
    }

}

int main() {
    //rapidjson::Document Config;
 //   Config.Parse(R"({
 //   "Folder" : "march7",
 //   "Name" : "march7",
 //   "Type" : "RVC",
 //   "Rate" : 48000,
 //   "Hop" : 320,
 //   "Cleaner" : "",
 //   "Hubert": "hubert4.0",
 //   "Diffusion": false,
 //   "CharaMix": false,
 //   "Volume": false,
 //   "HiddenSize": 256,
 //   "Characters" : ["march7"]
	//})");

    

    const auto model = TryCreateModel();


    //modify duration per phoneme
    InferClass::TTS::DurationCallback DurationCallback = [](std::vector<float>&) {};

    std::wstring exit = L"exit";
    std::vector<int16_t> output;
    try
    {
        //std::wstring inp;
        do
        {
            std::wstring input;
            std::wstring outpath;

            std::wcout << "�ȴ�������Ƶ·����" << std::endl;
            std::wcin >> input;

            if (input.empty() || input.size() == 0) {
                continue;
            }

            if (input.compare(exit) == 0 ) {
                break;
            }
            std::wcout << "�ȴ������Ƶ·����" << std::endl;
            std::wcin >> outpath;

            if (outpath.empty() || outpath.size() == 0) {
                continue;
            }
            // ��������Ը�������ָ�������Ӧ�Ĵ���
            //std::wcout << "����ָ��Ϊ��" << input << std::endl;


            // ��¼��ʼʱ��
            auto start = std::chrono::high_resolution_clock::now();
            output = model->Inference(input);


            const Wav outWav(model->GetSamplingRate(), output.size() * 2, output.data());
            outWav.Writef(outpath);
            std::wcout << "������Ƶ·����" << outpath << std::endl;

            // ��¼����ʱ��
            auto end = std::chrono::high_resolution_clock::now();

            // ����ִ��ʱ��
            std::chrono::duration<double, std::milli> elapsed = end - start;

            // ���ִ��ʱ��
            std::cout << "����ִ��ʱ��: " << elapsed.count() << " ms" << std::endl;

        } while (true);

    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }

    delete model;

    std::cout << "MoeVioce Done " << std::endl;
}