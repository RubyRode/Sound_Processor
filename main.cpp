#include <iostream>
#include <cstdio>
#include <filesystem>
#include "Utility/Input_parser.h"
#include "Proc/Proccessor.h"

using namespace std;
namespace fs = std::filesystem;

int getFileSize(FILE *inFile) {
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}

void foo(int start, int end, FILE *out, long long &offset) {
    int diff = end - start;
    uint32_t FILL_SIZE = 88200 * diff;
    int8_t fill[FILL_SIZE];

    for (int8_t &cell: fill) {
        cell = 0;
    }
    fseek(out, offset + start * 88200, SEEK_SET);

    fwrite(&fill, 1, FILL_SIZE, out);

}

void mix(FILE *out, long long &offset, long long &offset2, FILE *input, int start) {
//    int data1_size = getFileSize(out) - int(offset);
//    int data2_size = getFileSize(input) - int(offset2);


    uint32_t FILL_SIZE = 0;

//    if (data1_size<=data2_size) FILL_SIZE=data1_size;
//    else FILL_SIZE=data2_size;

    int16_t fill[44100 * FILL_SIZE];


    out->_offset = offset + start * 88200;
    //fseek(fout, offset + start*88200, SEEK_SET);
    //fseek(input, offset, SEEK_SET);
    input->_offset = offset;


    int16_t data1_chunk;
    int16_t data2_chunk;

    for (int16_t &cell: fill) {
        fread(&data2_chunk, 2, 1, input);
        fread(&data1_chunk, size_t(2), size_t(1), out);

        cell = (data1_chunk + data2_chunk) / 2;
    }

    fseek(out, offset + start * 88200, SEEK_SET);

    fwrite(&fill, 1, FILL_SIZE, out);
}

typedef struct sound {
    wav_hdr header;
    int track_data_size;
} sound;

sound get_header(FILE *f) {
    wav_hdr head;
    uint32_t header_size = sizeof(&head);
    fread(&head, header_size, 1, f);
    uint32_t track_info_size = head.Subchunk2Size + 8;
    char track_info[track_info_size];
    fread(&track_info, 1, track_info_size, f);
    char data_s[4];
    fread(&data_s, 1, 4, f);
    uint32_t sound_data_size;
    fread(&sound_data_size, 1, sizeof(sound_data_size), f);
    char sound_data[sound_data_size];
    fread(&sound_data, 1, sound_data_size, f);
    sound s;
    s.header = head;
    s.track_data_size = header_size + sound_data_size + 8 + track_info_size;
    return s;
}

int main(int argc, char **argv) {
    input_parser prs(argc, argv);

    ifstream ifs;

    vector <string> config_strings;
    vector <command> config_commands;
//    string config_strings[100];
//    command_ config_commands[100];
    int cnt_comm = -1;

    int len = prs.config_path.size();

    if (prs.config_path.empty()) {

        cerr << "Usage: " << argv[0] << " -c <config.txt> <output.wav> <input1.wav> [<input2.wav> …]" << std::endl;

    } else if (prs.config_path[len - 1] == 't' and prs.config_path[len - 2] == 'x' and
               prs.config_path[len - 3] == 't') {

        ifs.open(prs.config_path);
        read_config_file(ifs, &config_strings);
        cnt_comm = input_parser::parsed_config_lines(config_strings, &config_commands);

    }
    ifs.close();

    if (cnt_comm != -1) {
        wav_hdr orig_wav_head;
        FILE *orig_wav = fopen(prs.input_files[0].c_str(), "rb");


        sound orig_wav_header = get_header(orig_wav);
        orig_wav_head = orig_wav_header

        fs::copy("./" + prs.input_files[0], "./" + prs.output_wav, fs::copy_options::update_existing);
        FILE *output = fopen(prs.output_wav.c_str(), "rb+");

        long long offset = track_data_size;

        for (int i = 0; i < cnt_comm; i++) {

            size_t pos_mu = config_commands[i].command_.find("mute");
            size_t pos_mi = config_commands[i].command_.find("mix");

            if (pos_mu != string::npos) {
                command::parse_command(config_commands[i]);

                foo(config_commands[i].start, config_commands[i].end, output, offset);

            } else if (pos_mi != string::npos) {

                command::parse_command(config_commands[i]);
                cout << config_commands[i].num_file << endl;
                cout << prs.input_files[config_commands[i].num_file].c_str() << endl;
                FILE *input2 = fopen(prs.input_files[0].c_str(), "rb");
                wav_hdr input_head;
                long long offset2 = get_header(&input_head, input2);
                mix(output, offset, offset2, input2, config_commands[i].start);

            }

        }
        fclose(orig_wav);
        fclose(output);

    }
    cout << config_commands[0].command_ << endl;

    return 0;
}
