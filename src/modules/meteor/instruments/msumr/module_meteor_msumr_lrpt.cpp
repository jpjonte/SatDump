#include "module_meteor_msumr_lrpt.h"
#include <fstream>
#include "modules/meteor/simpledeframer.h"
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "logger.h"
#include <filesystem>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include <cstring>
#include "lrpt_msumr_reader.h"

#define BUFFER_SIZE 8192

// Return filesize
size_t getFilesize(std::string filepath);

namespace meteor
{
    namespace msumr
    {
        METEORMSUMRLRPTDecoderModule::METEORMSUMRLRPTDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters) : ProcessingModule(input_file, output_file_hint, parameters)
        {
        }

        void METEORMSUMRLRPTDecoderModule::process()
        {
            size_t filesize = getFilesize(d_input_file);
            std::ifstream data_in(d_input_file, std::ios::binary);

            std::string directory = d_output_file_hint.substr(0, d_output_file_hint.rfind('/')) + "/MSU-MR";

            logger->info("Using input frames " + d_input_file);
            logger->info("Decoding to " + directory);

            time_t lastTime = 0;

            libccsds::CADU cadu;

            libccsds::Demuxer ccsds_demuxer(882, true);

            lrpt::MSUMRReader msureader;

            logger->info("Demultiplexing and deframing...");

            while (!data_in.eof())
            {
                data_in.read((char *)&cadu, 1024);

                libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

                if (vcdu.vcid == 5)
                {
                    std::vector<libccsds::CCSDSPacket> frames = ccsds_demuxer.work(cadu);
                    for (libccsds::CCSDSPacket pkt : frames)
                    {
                        msureader.work(pkt);
                    }
                }

                if (time(NULL) % 10 == 0 && lastTime != time(NULL))
                {
                    lastTime = time(NULL);
                    logger->info("Progress " + std::to_string(round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f) + "%");
                }
            }

            data_in.close();

            //logger->info("MSU-MR Lines          : " + std::to_string(reader.lines));

            logger->info("Writing images.... (Can take a while)");

            if (!std::filesystem::exists(directory))
                std::filesystem::create_directory(directory);

            for (int channel = 0; channel < 6; channel++)
            {
                logger->info("Channel " + std::to_string(channel + 1) + "...");
                WRITE_IMAGE(msureader.getChannel(channel), directory + "/MSU-MR-" + std::to_string(channel + 1) + ".png");
            }

            {
                std::array<int32_t, 3> correlated = msureader.correlateChannels(0, 1);

                cimg_library::CImg<unsigned short> image1 = msureader.getChannel(0, correlated[0], correlated[1], correlated[2]);
                cimg_library::CImg<unsigned short> image2 = msureader.getChannel(1, correlated[0], correlated[1], correlated[2]);

                logger->info("221 Composite...");
                cimg_library::CImg<unsigned short> image221(image1.width(), image2.height(), 1, 3);
                {
                    image221.draw_image(0, 0, 0, 0, image2);
                    image221.draw_image(0, 0, 0, 1, image2);
                    image221.draw_image(0, 0, 0, 2, image1);
                }
                WRITE_IMAGE(image221, directory + "/MSU-MR-RGB-221.png");
                image221.equalize(1000);
                image221.normalize(0, std::numeric_limits<unsigned char>::max());
                WRITE_IMAGE(image221, directory + "/MSU-MR-RGB-221-EQU.png");
            }

            {
                std::array<int32_t, 3> correlated = msureader.correlateChannels(0, 1, 2);

                cimg_library::CImg<unsigned short> image1 = msureader.getChannel(0, correlated[0], correlated[1], correlated[2]);
                cimg_library::CImg<unsigned short> image2 = msureader.getChannel(1, correlated[0], correlated[1], correlated[2]);
                cimg_library::CImg<unsigned short> image3 = msureader.getChannel(2, correlated[0], correlated[1], correlated[2]);

                logger->info("321 Composite...");
                cimg_library::CImg<unsigned short> image321(image1.width(), image2.height(), 1, 3);
                {
                    image321.draw_image(0, 0, 0, 0, image3);
                    image321.draw_image(0, 0, 0, 1, image2);
                    image321.draw_image(0, 0, 0, 2, image1);
                }
                WRITE_IMAGE(image321, directory + "/MSU-MR-RGB-321.png");
                image321.equalize(1000);
                image321.normalize(0, std::numeric_limits<unsigned char>::max());
                WRITE_IMAGE(image321, directory + "/MSU-MR-RGB-321-EQU.png");
            }
        }

        std::string METEORMSUMRLRPTDecoderModule::getID()
        {
            return "meteor_msumr_lrpt";
        }

        std::vector<std::string> METEORMSUMRLRPTDecoderModule::getParameters()
        {
            return {};
        }

        std::shared_ptr<ProcessingModule> METEORMSUMRLRPTDecoderModule::getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters)
        {
            return std::make_shared<METEORMSUMRLRPTDecoderModule>(input_file, output_file_hint, parameters);
        }
    } // namespace msumr
} // namespace meteor