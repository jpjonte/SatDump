#pragma once

#include "module.h"
#include <complex>
#include <future>
#include "mpt_viterbi.h"
#include "modules/common/deframer.h"
#include <fstream>

namespace fengyun
{
    class FengyunMPTDecoderModule : public ProcessingModule
    {
    protected:
        int d_viterbi_outsync_after;
        float d_viterbi_ber_threasold;
        bool d_soft_symbols;

        uint8_t *viterbi_out;
        std::complex<float> *sym_buffer;
        int8_t *soft_buffer;

        // Work buffers
        uint8_t rsWorkBuffer[255];

        // Viterbi output buffer
        uint8_t *viterbi1_out;
        uint8_t *viterbi2_out;

        // A few buffers for processing
        std::complex<float> *iSamples, *qSamples;
        int inI = 0, inQ = 0;

        bool d_invert_second_viterbi;

        std::future<void> v1_fut, v2_fut;

        int v1, v2;

        int shift = 0, diffin = 0;

        // Diff decoder input and output
        uint8_t *diff_in, *diff_out;

        std::ifstream data_in;
        std::ofstream data_out;
        size_t filesize;

        FengyunMPTViterbi viterbi1, viterbi2;
        CADUDeframer deframer;

        int errors[4];

        // UI Stuff
        float ber_history1[200];
        float ber_history2[200];

    public:
        FengyunMPTDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
        void process();
        void drawUI();

    public:
        static std::string getID();
        static std::vector<std::string> getParameters();
        static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
    };
} // namespace fengyun