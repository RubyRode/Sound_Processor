#include <iostream>
#include <cstdio>
#include "Utility/Input_parser.h"
#include "Proc/Proccessor.h"

using namespace std;

int getFileSize(FILE* inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}

int main(int argc, char **argv) {
    input_parser prs(argc, argv);

    ifstream ifs;

    vector<string> config_strings;
    vector<command> config_commands;
//    string config_strings[100];
//    command_ config_commands[100];
    int cnt_comm = -1;

    int len = prs.config_path.size();

    if (prs.config_path.empty()){

        cerr << "Usage: " << argv[0] << " -c <config.txt> <output.wav> <input1.wav> [<input2.wav> …]" << std::endl;

    }else if (prs.config_path[len-1] == 't' and prs.config_path[len-2] == 'x' and prs.config_path[len-3] == 't'){

        ifs.open(prs.config_path);
        read_config_file(ifs, &config_strings);
        cnt_comm = input_parser::parsed_config_lines(config_strings, &config_commands);

    }
    ifs.close();

    if (cnt_comm != -1){
        wav_hdr orig_wav_head;
        FILE* orig_wav = fopen(prs.input_files[0].c_str(), "rb");

        int header_size = sizeof(orig_wav_head), filelength = 0;
        size_t bytes_read = fread(&orig_wav_head, 1, header_size, orig_wav);

        int track_info_size = orig_wav_head.Subchunk2Size+8;
        char track_info[track_info_size];
        fread(&track_info, 1, track_info_size, orig_wav);

        FILE* output = fopen(prs.output_wav.c_str(), "wb");
        fwrite(&orig_wav_head, 1, header_size, output);
        fwrite(&track_info, 1, track_info_size, output);
        fclose(output);

        static const uint32_t FILL_SIZE = 88200;
        int8_t fill[FILL_SIZE];
        for (int8_t & cell : fill){
            cell = 0;
        }
        for (int i = 0; i < cnt_comm; i++){
            output = fopen(prs.output_wav.c_str(), "w");
            fseek(output, header_size + track_info_size, SEEK_SET);

            size_t pos_mu = config_commands[i].command_.find("mute");
            size_t pos_mi = config_commands[i].command_.find("mix");

            if (pos_mu != string::npos){
                command::parse_command(config_commands[i]);


                //Read the data
                uint16_t bytesPerSample = orig_wav_head.bitsPerSample / 8;      //Number     of bytes per sample
                uint64_t numSamples = orig_wav_head.ChunkSize / bytesPerSample; //How many samples are in the wav file?

                static const uint32_t BUFFER_SIZE = 88200;
                auto* buffer = new int8_t[BUFFER_SIZE];
                int sec_cnt = 0;
                while ((bytes_read = fread(buffer, sizeof buffer[0], BUFFER_SIZE / (sizeof buffer[0]), orig_wav)) > 0)
                {
                    /** DO SOMETHING WITH THE WAVE DATA HERE **/
                    if (sec_cnt >= config_commands[i].start && sec_cnt < config_commands[i].end){
                        fwrite(fill, sizeof fill[0], FILL_SIZE / sizeof(fill[0]), output);
                    }else {
                        fwrite(buffer, sizeof buffer[0], BUFFER_SIZE / sizeof(buffer[0]), output);
                    }
                    cout << "Read " << bytes_read << " bytes." << endl;
                    sec_cnt++;
                }
                delete [] buffer;
                buffer = nullptr;
                filelength = getFileSize(orig_wav);

                //orig_wav = output;
                fclose(output);

            }else if (pos_mi != string::npos){

                command::parse_command(config_commands[i]);

            }

        }
        fclose(orig_wav);

    }
    cout << config_commands[0].command_ << endl;

    return 0;
}
