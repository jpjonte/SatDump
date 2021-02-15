#pragma once

#include "module.h"

namespace fengyun
{
    namespace mersi1
    {
        class FengyunMERSI1DecoderModule : public ProcessingModule
        {
        protected:
        
        public:
            FengyunMERSI1DecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
            void process();

        public:
            static std::string getID();
            static std::vector<std::string> getParameters();
            static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
        };
    } // namespace virr
} // namespace fengyun