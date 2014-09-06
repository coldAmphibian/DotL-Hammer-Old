/*
 * This entire file is taken from this post: http://forum.xentax.com/viewtopic.php?p=92650#p92650
 * 
 * Some changes have been made including the ability to specify the filename on the command line
 * and the removal of the windows specific code.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <math.h>
#include <float.h>
#include <iomanip>

using namespace std;

class nvr_vertex {
public:
    float position[3];
    float normals[3];
    float uv[2];
    int unknown;
};

class nvr_material_struct_class {
public:
    string name;
    float emissive_color[3];
    float blend_color[4];
    string texture_filename;
    float opacity;
    string blend_filename;
    int used;
};

class nvr_vertexlist_struct_class {
public:
    int size;
    int type;
    nvr_vertex *vertices;
};

class nvr_indexlist_struct_class {
public:
    int size;
    unsigned int d3dfmt;
    unsigned short *indices;
};

class nvr_model_model_struct_class {
public:
    int vertex_index;
    int vertex_offset;
    int vertex_length;
    int index_index;
    int index_offset;
    int index_length;
};

class nvr_model_struct_class {
public:
    int flag_1;
    int flag_2;
    float b[10];
    int material;
    nvr_model_model_struct_class model[2];
};

class nvr_unknown3_struct_class {
public:
    float a[6];
    int b[4];
};

class nvr_struct_class {
public:
    string magic;
    unsigned short unknown_1;
    unsigned short unknown_2;
    unsigned int count_material;
    unsigned int count_vertex_list;
    unsigned int count_index_list;
    unsigned int count_model;
    unsigned int count_unknown_3;

    nvr_material_struct_class * materials;
    nvr_vertexlist_struct_class * vertex_lists;
    nvr_indexlist_struct_class * index_lists;
    nvr_model_struct_class * models;
    nvr_unknown3_struct_class * unknown_3;

};

nvr_struct_class nvr_struct;
char * nvr_data;

void _ReadNext(char *buffer, int bytes, bool seek = false);
string _MemToString(char *buffer);
unsigned short _MemToShort(char *buffer);
unsigned int _MemToInt(char *buffer);
float _MemToFloat(char *buffer);
float _MemToFloat2(char *buffer);

string _XFileHead();

string _NvrHeadToString(nvr_struct_class &nvr);
string _NvrMaterialToString(nvr_material_struct_class material);
string _NvrVertexToString(nvr_vertex vertex);
string _NvrVertexToString2(nvr_vertex vertex);
string _NvrModelToString(nvr_model_struct_class model);

float round(float r, int places);

int main(int argc, char** argv) {

    char* file;

    if (argc >= 2) {
        file = argv[1];
    } else {
        file = (char*) "room.nvr";
    }

    cout << "\nNVR Converter        Version 1.0\n\nA room.nvr file has to be located next to this exe. \nExtracted files can be found in the output folder. \nMatching texture files are located in \"[MapPath]\\Schens\\Textures\"\nThe default texture path is \"teture\\[name].dds\", relative to the programm loading it. \nNo customization of these parameter currently possible.\n\n\n\n";

    cout << "Press ENTER to continue...";
    cin.ignore();


    cout << "Loading file into memory... ";
    ifstream nvr_file;
    nvr_file.open("room.nvr", ios::binary | ios::in | ios::ate);
    int size = nvr_file.tellg();
    if (size == -1) {
        cout << "error loading file\n\n";
        return 0;
    }
    nvr_data = new char [size];
    nvr_file.seekg(0, ios::beg);
    nvr_file.read(nvr_data, size);
    nvr_file.close();
    cout << "Finished\n\n";

    cout << "Resolving file structure: \n\n";

    char buffer_1[4], buffer_2[2], buffer_3[256], buffer_4[336], buffer_5[2364];

    //get head data
    _ReadNext(buffer_1, 4);
    nvr_struct.magic = _MemToString(buffer_1); //magic number (4 byte char array)
    _ReadNext(buffer_2, 2);
    nvr_struct.unknown_1 = _MemToShort(buffer_2); //unknown value (2 byte short)
    _ReadNext(buffer_2, 2);
    nvr_struct.unknown_2 = _MemToShort(buffer_2); //unknown value (2 byte short)
    _ReadNext(buffer_1, 4);
    nvr_struct.count_material = _MemToInt(buffer_1); //material count (4 byte int)
    _ReadNext(buffer_1, 4);
    nvr_struct.count_vertex_list = _MemToInt(buffer_1); //vertex list count (4 byte int)
    _ReadNext(buffer_1, 4);
    nvr_struct.count_index_list = _MemToInt(buffer_1); //index list count (4 byte int)
    _ReadNext(buffer_1, 4);
    nvr_struct.count_model = _MemToInt(buffer_1); //model count (4 byte int)
    _ReadNext(buffer_1, 4);
    nvr_struct.count_unknown_3 = _MemToInt(buffer_1); //unknown3 count (4 byte int)
    //display result
    cout << _NvrHeadToString(nvr_struct);


    //get material data
    cout << "  materials [" << nvr_struct.count_material << "]      { Processing... ";
    nvr_struct.materials = new nvr_material_struct_class[nvr_struct.count_material];
    for (int i = 0; i < nvr_struct.count_material; i++) {
        _ReadNext(buffer_3, 256);
        nvr_struct.materials[i].name = _MemToString(buffer_3); //name (256 byte char array)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].emissive_color[0] = _MemToFloat(buffer_1); //emissive color Red (4 byte float)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].emissive_color[1] = _MemToFloat(buffer_1); //emissive color Green (4 byte float)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].emissive_color[2] = _MemToFloat(buffer_1); //emissive color Blue (4 byte float)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].blend_color[0] = _MemToFloat(buffer_1); //blend color Alpha (4 byte float)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].blend_color[1] = _MemToFloat(buffer_1); //blend color Red (4 byte float)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].blend_color[2] = _MemToFloat(buffer_1); //blend color Green (4 byte float)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].blend_color[3] = _MemToFloat(buffer_1); //blend color Blue (4 byte float)
        _ReadNext(buffer_4, 336);
        nvr_struct.materials[i].texture_filename = _MemToString(buffer_4); //texture file name (336 byte char array)
        _ReadNext(buffer_1, 4);
        nvr_struct.materials[i].opacity = _MemToFloat(buffer_1); //opacity (4 byte float)
        _ReadNext(buffer_5, 2364);
        nvr_struct.materials[i].blend_filename = _MemToString(buffer_5); //blend file name (2364 byte char array)
        nvr_struct.materials[i].used = 0; //used 0 times (reset)
    }
    cout << "Finished }\n\n";


    //get vertex list
    cout << "  vertex_lists [" << nvr_struct.count_vertex_list << "] {\n\n";
    nvr_struct.vertex_lists = new nvr_vertexlist_struct_class[nvr_struct.count_vertex_list];

    for (int i = 0; i < nvr_struct.count_vertex_list; i++) {
        _ReadNext(buffer_1, 4);
        int temp_size = _MemToInt(buffer_1); //size of current vertex list
        int temp_values[4];


        _ReadNext(NULL, 12, true);
        _ReadNext(buffer_1, 4);
        temp_values[0] = _MemToFloat(buffer_1);
        _ReadNext(buffer_1, 4);
        temp_values[1] = _MemToFloat(buffer_1);
        _ReadNext(buffer_1, 4);
        temp_values[2] = _MemToFloat(buffer_1);

        _ReadNext(NULL, 12, true);
        _ReadNext(buffer_1, 4);
        temp_values[3] = _MemToInt(buffer_1);
        _ReadNext(NULL, -40, true);

        if (temp_size / 36 * 36 == temp_size && temp_values[0] <= 1 && temp_values[1] <= 1 && temp_values[2] <= 1) {
            nvr_struct.vertex_lists[i].size = temp_size / 36;
            nvr_struct.vertex_lists[i].type = 1;
        } else
            if ((temp_values[3] == 0xFF7F7F7F) || (temp_values[3] == 0xFF16191E)) {
            nvr_struct.vertex_lists[i].size = temp_size / 40;
            nvr_struct.vertex_lists[i].type = 2;
        } else {
            nvr_struct.vertex_lists[i].size = temp_size / 12;
            nvr_struct.vertex_lists[i].type = 0;
        }


        cout << "     size:  " << nvr_struct.vertex_lists[i].size
                << "   type: " << nvr_struct.vertex_lists[i].type
                << "      vertices_" << i << " [" << nvr_struct.vertex_lists[i].size << "]     { Processing... ";

        nvr_struct.vertex_lists[i].vertices = new nvr_vertex[nvr_struct.vertex_lists[i].size];

        if (nvr_struct.vertex_lists[i].type > 0) {

            for (int j = 0; j < nvr_struct.vertex_lists[i].size; j++) {
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[0] = _MemToFloat(buffer_1); //x position of current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[1] = _MemToFloat(buffer_1); //y position of current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[2] = _MemToFloat(buffer_1); //z position of current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].normals[0] = _MemToFloat(buffer_1); //x component of normal on current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].normals[1] = _MemToFloat(buffer_1); //y component of normal on current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].normals[2] = _MemToFloat(buffer_1); //z component of normal on current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].uv[0] = _MemToFloat(buffer_1); //u coordinate on texture
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].uv[1] = _MemToFloat(buffer_1); //v coordinate on texture
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].unknown = _MemToInt(buffer_1); //unknown value
                if (nvr_struct.vertex_lists[i].type == 2) _ReadNext(buffer_1, 4);
            }
            cout << "Finished }\n";

        } else {

            for (int j = 0; j < nvr_struct.vertex_lists[i].size; j++) {
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[0] = _MemToFloat(buffer_1); //x position of current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[1] = _MemToFloat(buffer_1); //y position of current vertex
                _ReadNext(buffer_1, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[2] = _MemToFloat(buffer_1); //z position of current vertex
            }
            cout << "Finished }\n";

        }
    }
    cout << "  }\n\n";

    //get index list
    cout << "  index_lists[" << nvr_struct.count_index_list << "] {\n\n";
    nvr_struct.index_lists = new nvr_indexlist_struct_class[nvr_struct.count_index_list];

    for (int i = 0; i < nvr_struct.count_index_list; i++) {
        _ReadNext(buffer_1, 4);
        nvr_struct.index_lists[i].size = _MemToInt(buffer_1) / 2; //size of current index list
        _ReadNext(buffer_1, 4);
        nvr_struct.index_lists[i].d3dfmt = _MemToShort(buffer_1); //d3dformat

        cout << "     size:  " << nvr_struct.index_lists[i].size
                << "    d3dformat: " << nvr_struct.index_lists[i].d3dfmt
                << "      indices_" << i << " [" << nvr_struct.index_lists[i].size << "]        { Processing... ";

        nvr_struct.index_lists[i].indices = new unsigned short[nvr_struct.index_lists[i].size];

        for (int j = 0; j < nvr_struct.index_lists[i].size; j++) {
            _ReadNext(buffer_2, 2);
            nvr_struct.index_lists[i].indices[j] = _MemToShort(buffer_2); //vertex id
        }
        cout << "Finished }\n";
    }
    cout << "  }\n\n";

    //get models
    cout << "  models[" << nvr_struct.count_model << "]             { Processing... ";
    nvr_struct.models = new nvr_model_struct_class[nvr_struct.count_model];

    for (int i = 0; i < nvr_struct.count_model; i++) {
        _ReadNext(buffer_1, 4);
        nvr_struct.models[i].flag_1 = _MemToInt(buffer_1); //flag 1
        _ReadNext(buffer_1, 4);
        nvr_struct.models[i].flag_2 = _MemToInt(buffer_1); //flag 2
        for (int j = 0; j < 10; j++) {
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].b[j] = _MemToFloat(buffer_1); //b
        }
        _ReadNext(buffer_1, 4);
        nvr_struct.models[i].material = _MemToInt(buffer_1); //material
        for (int j = 0; j < 2; j++) {
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].model[j].vertex_index = _MemToInt(buffer_1); //vertex index
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].model[j].vertex_offset = _MemToInt(buffer_1); //vertex offset
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].model[j].vertex_length = _MemToInt(buffer_1); //vertex length
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].model[j].index_index = _MemToInt(buffer_1); //index index
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].model[j].index_offset = _MemToInt(buffer_1); //index offset
            _ReadNext(buffer_1, 4);
            nvr_struct.models[i].model[j].index_length = _MemToInt(buffer_1); //index length
        }

    }
    cout << "Finished }\n\n";

    //get unknown_3s
    cout << "  unknown_3s[" << nvr_struct.count_unknown_3 << "]     { Processing... ";
    nvr_struct.unknown_3 = new nvr_unknown3_struct_class[nvr_struct.count_unknown_3];

    for (int i = 0; i < nvr_struct.count_unknown_3; i++) {
        for (int j = 0; j < 6; j++) {
            _ReadNext(buffer_1, 4);
            nvr_struct.unknown_3[i].a[j] = _MemToFloat(buffer_1); //a
        }
        for (int j = 0; j < 4; j++) {
            _ReadNext(buffer_1, 4);
            nvr_struct.unknown_3[i].b[j] = _MemToInt(buffer_1); //b
        }
    }

    cout << "Finished }\n\n}\n\n\n";

    //convert to .x files
    cout << "Converting " << nvr_struct.count_model << " models to X Format...\n\n";

    for (int i = 0; i < nvr_struct.count_model; i++) {

        int XFile_vertex_index = nvr_struct.models[i].model[0].vertex_index;
        int XFile_vertex_offset = nvr_struct.models[i].model[0].vertex_offset;
        int XFile_vertex_length = nvr_struct.models[i].model[0].vertex_length;

        int XFile_index_index = nvr_struct.models[i].model[0].index_index;
        int XFile_index_offset = nvr_struct.models[i].model[0].index_offset;
        int XFile_index_length = nvr_struct.models[i].model[0].index_length;

        string XFile_texture = nvr_struct.materials[nvr_struct.models[i].material].texture_filename;

        stringstream ss; // << "output\\" 
        ss << nvr_struct.materials[nvr_struct.models[i].material].name << nvr_struct.materials[nvr_struct.models[i].material].used << ".x";
        string xFile_name = ss.str();
        nvr_struct.materials[nvr_struct.models[i].material].used += 1;

        cout << "Converting Model " << i << "/" << nvr_struct.count_model << "          " << xFile_name << "\n";


        stringstream ssXFile;
        ssXFile << fixed << showpoint << setprecision(6);

        ssXFile << _XFileHead() << "\n";
        ssXFile << "Mesh {\n";

        //positions
        ssXFile << " " << XFile_vertex_length << ";\n";
        for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
            if (j < XFile_vertex_offset + XFile_vertex_length - 1) {
                ssXFile << " " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].position[0] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].position[1] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].position[2] << ";,\n";
            } else {
                ssXFile << " " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].position[0] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].position[1] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].position[2] << ";;\n\n";
            }
        }
        //indices for positions
        ssXFile << " " << ((int) XFile_index_length / 3) << ";\n";
        for (int j = XFile_index_offset; j < XFile_index_offset + XFile_index_length; j += 3) {
            if (j < XFile_index_offset + XFile_index_length - 3) {
                ssXFile << " 3;" << (nvr_struct.index_lists[XFile_index_index].indices[j] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 1] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 2] - XFile_vertex_offset) << ";,\n";
            } else {
                ssXFile << " 3;" << (nvr_struct.index_lists[XFile_index_index].indices[j] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 1] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 2] - XFile_vertex_offset) << ";;\n\n";
            }
        }
        //normals
        ssXFile << " MeshNormals {\n  " << XFile_vertex_length << ";\n";
        for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
            if (j < XFile_vertex_offset + XFile_vertex_length - 1) {
                ssXFile << "  " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[0] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[1] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[2] << ";,\n";
            } else {
                ssXFile << "  " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[0] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[1] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[2] << ";;\n\n";
            }
        }
        //indices for normals
        ssXFile << "  " << ((int) XFile_index_length / 3) << ";\n";
        for (int j = XFile_index_offset; j < XFile_index_offset + XFile_index_length; j += 3) {
            if (j < XFile_index_offset + XFile_index_length - 3) {
                ssXFile << "  3;" << (nvr_struct.index_lists[XFile_index_index].indices[j] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 1] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 2] - XFile_vertex_offset) << ";,\n";
            } else {
                ssXFile << "  3;" << (nvr_struct.index_lists[XFile_index_index].indices[j] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 1] - XFile_vertex_offset) << ","
                        << (nvr_struct.index_lists[XFile_index_index].indices[j + 2] - XFile_vertex_offset) << ";;\n }\n\n";
            }
        }
        //texture coodrinates
        ssXFile << " MeshTextureCoords {\n  " << XFile_vertex_length << ";\n";
        for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
            if (j < XFile_vertex_offset + XFile_vertex_length - 1) {
                ssXFile << "  " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[0] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[1] << ";,\n";
            } else {
                ssXFile << "  " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[0] << ";"
                        << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[0] << ";;\n }\n\n";
            }
        }
        //material list
        ssXFile << " MeshMaterialList {\n  1;\n  1;\n  0;\n\n";
        //material
        ssXFile << "  Material {\n"
                << "   1.000000;1.000000;1.000000;1.000000;;\n"
                << "   3.200000;\n"
                << "   0.000000;0.000000;0.000000;;\n"
                << "   0.000000;0.000000;0.000000;;\n\n";
        //texutre
        ssXFile << "   TextureFilename {\n"
                << "    \"texture\\\\" << XFile_texture << "\";\n   }\n  }\n }\n}";

        //save to file
        ofstream XFile;
        XFile.open(xFile_name.c_str());
        XFile << ssXFile.str();
        XFile.close();
    }
    return 0;
    while (1);
}

void _ReadNext(char *buffer, int bytes, bool seek) {
    static int pos = 0;

    if (!seek) {
        for (int i = 0; i < bytes; i++) {
            buffer[i] = nvr_data[i + pos];
        }
    }

    pos += bytes;
    return;
}

string _MemToString(char *buffer) {
    string ret = buffer;
    return ret;
}

unsigned short _MemToShort(char *buffer) {
    return *((short*) buffer);
    //return ((0xFF00&(buffer[1]<<8))|(0x00FF&buffer[0]));
}

unsigned int _MemToInt(char *buffer) {
    return *((int*) buffer);
    //return ((0xFF000000&(buffer[3]<<24))|(0x00FF0000&(buffer[2]<<16))|(0x0000FF00&(buffer[1]<<8))|(0x000000FF&buffer[0]));
}

float _MemToFloat2(char *buffer) {
    int full = ((0xFF000000 & (buffer[3] << 24)) | (0x00FF0000 & (buffer[2] << 16)) | (0x0000FF00 & (buffer[1] << 8)) | (0x000000FF & buffer[0]));
    int sign = 0x00000001 & (full >> 31);
    int exponent = 0x000000FF & (full >> 23);
    int mantissa = 0x007FFFFF & full;
    float ret = round(pow(-1.0f, sign)*(1.0 + ((float) mantissa) / 8388608) * pow(2.0f, exponent - 127), 6);

    if (!isfinite(ret)) {
        ret = 0;
    } //catch NaN and infinite
    return ret;
}

float _MemToFloat(char *buffer) {
    return *((float*) buffer);
}

string _NvrHeadToString(nvr_struct_class &nvr) {
    stringstream ss;
    ss << "NVR {\n\n"
            << "  magic:            " << nvr.magic << "\n"
            << "  unknown_1:                " << nvr.unknown_1 << "\n"
            << "  unknown_2:                " << nvr.unknown_2 << "\n"
            << "  count_material:   " << nvr.count_material << "\n"
            << "  count_vertex_list:        " << nvr.count_vertex_list << "\n"
            << "  count_index_list: " << nvr.count_index_list << "\n"
            << "  count_model:              " << nvr.count_model << "\n"
            << "  count_unknown_3:  " << nvr.count_unknown_3 << "\n\n";

    return ss.str();
}

string _NvrMaterialToString(nvr_material_struct_class material) {
    stringstream ss;
    ss << "    " <<
            material.name << "|" <<
            material.emissive_color[0] << "|" << material.emissive_color[1] << "|" << material.emissive_color[2] << "|" <<
            material.blend_color[0] << "|" << material.blend_color[1] << "|" << material.blend_color[2] << "|" << material.blend_color[3] << "|" <<
            material.texture_filename << "|" <<
            material.opacity << "|" <<
            material.blend_filename << "\n";

    return ss.str();
}

string _NvrVertexToString(nvr_vertex vertex) {
    stringstream ss;
    ss << "    " <<
            vertex.position[0] << "|" << vertex.position[1] << "|" << vertex.position[2] << "|" <<
            vertex.normals[0] << "|" << vertex.normals[1] << "|" << vertex.normals[2] << "|" <<
            vertex.uv[0] << "|" << vertex.uv[1] << "|" <<
            vertex.unknown << "\n";

    return ss.str();
}

string _NvrVertexToString2(nvr_vertex vertex) {
    stringstream ss;
    ss << "    " <<
            vertex.position[0] << "|" << vertex.position[1] << "|" << vertex.position[2] << "\n";

    return ss.str();
}

/*string _NvrIndexToString(nvr_index index) {
        stringstream ss;
        ss <<   "    " <<
                        index.id[0] <<"|"<< index.id[1] <<"|"<< index.id[2] <<"\n";
     
        return ss.str();
}*/

string _NvrModelToString(nvr_model_struct_class model) {
    stringstream ss;
    ss << "    " <<
            model.flag_1 << "|" << model.flag_2 << "|" << model.material << "|" <<
            model.model[0].vertex_index << "|" << model.model[0].vertex_offset << "|" << model.model[0].vertex_length << "|" << model.model[0].index_index << "|" << model.model[0].index_offset << "|" << model.model[0].index_length << "|" <<
            model.model[1].vertex_index << "|" << model.model[1].vertex_offset << "|" << model.model[1].vertex_length << "|" << model.model[1].index_index << "|" << model.model[1].index_offset << "|" << model.model[1].index_length << "|" <<
            "\n";

    return ss.str();
}

float round(float r, int places) {
    return floor(pow(10.0f, places) * r + 0.5f) / pow(10.0f, places);
}

string _XFileHead() {
    string ret;
    ret = ""
            "xof 0303txt 0032\n"
            "template Vector {\n"
            " <3d82ab5e-62da-11cf-ab39-0020af71e433>\n"
            " FLOAT x;\n"
            " FLOAT y;\n"
            " FLOAT z;\n"
            "}\n"
            "\n"
            "template MeshFace {\n"
            " <3d82ab5f-62da-11cf-ab39-0020af71e433>\n"
            " DWORD nFaceVertexIndices;\n"
            " array DWORD faceVertexIndices[nFaceVertexIndices];\n"
            "}\n"
            "\n"
            "template Mesh {\n"
            " <3d82ab44-62da-11cf-ab39-0020af71e433>\n"
            " DWORD nVertices;\n"
            " array Vector vertices[nVertices];\n"
            " DWORD nFaces;\n"
            " array MeshFace faces[nFaces];\n"
            " [...]\n"
            "}\n"
            "\n"
            "template MeshNormals {\n"
            " <f6f23f43-7686-11cf-8f52-0040333594a3>\n"
            " DWORD nNormals;\n"
            " array Vector normals[nNormals];\n"
            " DWORD nFaceNormals;\n"
            " array MeshFace faceNormals[nFaceNormals];\n"
            "}\n"
            "\n"
            "template Coords2d {\n"
            " <f6f23f44-7686-11cf-8f52-0040333594a3>\n"
            " FLOAT u;\n"
            " FLOAT v;\n"
            "}\n"
            "\n"
            "template MeshTextureCoords {\n"
            " <f6f23f40-7686-11cf-8f52-0040333594a3>\n"
            " DWORD nTextureCoords;\n"
            " array Coords2d textureCoords[nTextureCoords];\n"
            "}\n"
            "\n"
            "template Material {\n"
            " <3d82ab4d-62da-11cf-ab39-0020af71e433>\n"
            " ColorRGBA faceColor;\n"
            " FLOAT power;\n"
            " ColorRGB specularColor;\n"
            " ColorRGB emissiveColor;\n"
            " [...]\n"
            "}\n"
            "\n"
            "template MeshMaterialList {\n"
            " <f6f23f42-7686-11cf-8f52-0040333594a3>\n"
            " DWORD nMaterials;\n"
            " DWORD nFaceIndexes;\n"
            " array DWORD faceIndexes[nFaceIndexes];\n"
            " [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]\n"
            "}\n"
            "\n"
            "template TextureFilename {\n"
            " <a42790e1-7810-11cf-8f52-0040333594a3>\n"
            " STRING filename;\n"
            "}\n";

    return ret;
}