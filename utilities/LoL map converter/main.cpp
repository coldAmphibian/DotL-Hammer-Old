/*
 * The code in this file is derived from the code avaliable:
 *   http://forum.xentax.com/viewtopic.php?p=92650#p92650
 *
 * The code has been modified to produce `.obj` and `.mtl` files in a platform
 *  independent manner.
 *
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <math.h>
#include <float.h>
#include <iomanip>
#include <regex>

using namespace std;

class nvr_vertex {
public:
    float position[3];
    float normals[3];
    float uv[2];
//    int unknown;
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
static int pos = 0;
//std::regex rx("jungle.*island_a");

void check_name(string name);

bool _CFR(float f) { // CheckFloatRange
    return f < -10000000000000000 || f > 10000000000000000;
}

string byte_to_binary(char x)
{
    string b = "";
    for (int z = 128; z > 0; z >>= 1) {
        b += ((x & z) == z) ? "1" : "0";
    }
    return b;
}

void print_data(char* buf, int len){
    cout << "(" << len << ")     ";
    for(int i = 0; i < len; i++) {
        cout << byte_to_binary(buf[i]) << " ";
    }
    cout << "|\n";
}

void print_data_r(char* buf, int len){
    cout << "(" << len << ") (S) ";
    for(int i = len - 1; i >= 0; i--) {
        cout << byte_to_binary(buf[i]) << " ";
    }
    cout << "|\n";
}


void _ReadNext(char *buffer, int bytes, bool seek = false);
string _MemToString(char *buffer);
unsigned short _MemToShort(char *buffer);
unsigned int _MemToInt(char *buffer);
float _MemToFloat(char *buffer);
float _MemToFloat2(char *buffer);

void _PrintStart(string name, int count);

string _GenHeader();
void _SaveData(nvr_struct_class nvr_struct, int i);

float round(float r, int places);

int main(int argc, const char** argv) {
    bool magic_values;
    bool zeros_all;
    const char* file;

    if (argc >= 2) {
        file = argv[1];
    } else {
        file = "room.nvr";
    }

    cout << "LoL Map Converter\n";
    cout << "=================\n\n";
    cout << "This program is about to load and convert the following file: \n";
    cout << "  " << file << "\n\n";
    cout << "The generated `.obj` and `.mtl` files will be placed into the current working directory.\n\n";
    cout << "This program is based on code from:\n  http://forum.xentax.com/viewtopic.php?p=92650#p92650 \n\n\n";

    cout << "================================================================================\n\n\n";

    cout << "Loading file into memory... ";
    ifstream nvr_file;
    nvr_file.open(file, ios::binary | ios::in | ios::ate);
    int size = nvr_file.tellg();
    if (size == -1) {
        cout << "error loading file\n\n";
        return -1;
    }
    nvr_data = new char [size];
    nvr_file.seekg(0, ios::beg);
    nvr_file.read(nvr_data, size);
    nvr_file.close();
    cout << "Finished\n\n";

    cout << "Resolving file structure: \n\n";

    char buffer_2[2], buffer_4[4], buffer_256[256], buffer_336[336], buffer_2364[2364];

    //get head data
    _ReadNext(buffer_4, 4);
    nvr_struct.magic = _MemToString(buffer_4); //magic number (4 byte char array)
    _ReadNext(buffer_2, 2);
    nvr_struct.unknown_1 = _MemToShort(buffer_2); //unknown value (2 byte short)
    _ReadNext(buffer_2, 2);
    nvr_struct.unknown_2 = _MemToShort(buffer_2); //unknown value (2 byte short)
    _ReadNext(buffer_4, 4);
    nvr_struct.count_material = _MemToInt(buffer_4); //material count (4 byte int)
    _ReadNext(buffer_4, 4);
    nvr_struct.count_vertex_list = _MemToInt(buffer_4); //vertex list count (4 byte int)
    _ReadNext(buffer_4, 4);
    nvr_struct.count_index_list = _MemToInt(buffer_4); //index list count (4 byte int)
    _ReadNext(buffer_4, 4);
    nvr_struct.count_model = _MemToInt(buffer_4); //model count (4 byte int)
    _ReadNext(buffer_4, 4);
    nvr_struct.count_unknown_3 = _MemToInt(buffer_4); //unknown3 count (4 byte int)
    //display result


    //get material data
    _PrintStart("materials", nvr_struct.count_material);
    nvr_struct.materials = new nvr_material_struct_class[nvr_struct.count_material];
    for (int i = 0; i < nvr_struct.count_material; i++) {
        _ReadNext(buffer_256, 256);
        nvr_struct.materials[i].name = _MemToString(buffer_256); //name (256 byte char array)
        check_name(nvr_struct.materials[i].name);
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].emissive_color[0] = _MemToFloat(buffer_4); //emissive color Red (4 byte float)
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].emissive_color[1] = _MemToFloat(buffer_4); //emissive color Green (4 byte float)
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].emissive_color[2] = _MemToFloat(buffer_4); //emissive color Blue (4 byte float)
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].blend_color[0] = _MemToFloat(buffer_4); //blend color Alpha (4 byte float)
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].blend_color[1] = _MemToFloat(buffer_4); //blend color Red (4 byte float)
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].blend_color[2] = _MemToFloat(buffer_4); //blend color Green (4 byte float)
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].blend_color[3] = _MemToFloat(buffer_4); //blend color Blue (4 byte float)
        _ReadNext(buffer_336, 336);
        nvr_struct.materials[i].texture_filename = _MemToString(buffer_336); //texture file name (336 byte char array)
        check_name(nvr_struct.materials[i].texture_filename);
        _ReadNext(buffer_4, 4);
        nvr_struct.materials[i].opacity = _MemToFloat(buffer_4); //opacity (4 byte float)
        _ReadNext(buffer_2364, 2364);
        nvr_struct.materials[i].blend_filename = _MemToString(buffer_2364); //blend file name (2364 byte char array)
        check_name(nvr_struct.materials[i].blend_filename);
        nvr_struct.materials[i].used = 0; //used 0 times (reset)
    }
    cout << "Finished }\n";


    //get vertex list
    _PrintStart("vertex_lists", nvr_struct.count_vertex_list);
    nvr_struct.vertex_lists = new nvr_vertexlist_struct_class[nvr_struct.count_vertex_list];

    for (int i = 0; i < nvr_struct.count_vertex_list; i++) {
        cout << "\nStart: " << pos << "\n";
        _ReadNext(buffer_4, 4);
        int temp_size = _MemToInt(buffer_4); //size of current vertex list
        print_data(buffer_4, 4);
        int temp_values[4];


        _ReadNext(NULL, 12, true);
        _ReadNext(buffer_4, 4);
        temp_values[0] = _MemToFloat(buffer_4);
        print_data_r(buffer_4, 4);
        _ReadNext(buffer_4, 4);
        temp_values[1] = _MemToFloat(buffer_4);
        print_data_r(buffer_4, 4);
        _ReadNext(buffer_4, 4);
        temp_values[2] = _MemToFloat(buffer_4);
        print_data_r(buffer_4, 4);
// a1 3c 6c bd
// dc 72 69 3c
// 40 8c 7f bf
        _ReadNext(NULL, 12, true);
        _ReadNext(buffer_4, 4);
        temp_values[3] = _MemToInt(buffer_4);
        print_data(buffer_4, 4);
        _ReadNext(NULL, -40, true);

        cout << "Size: " << temp_size << "|| 0: " << temp_values[0] << " 1: " << temp_values[1] << " 2: " << temp_values[2] << " 3: " << temp_values[3] << "\n";

        magic_values = (temp_values[3] == 0xFF7F7F7F) || (temp_values[3] == 0xFF16191E);
        zeros_all = temp_values[0] <= 1 && temp_values[1] <= 1 && temp_values[2] <= 1;
        
        if (magic_values && zeros_all) {
            nvr_struct.vertex_lists[i].size = temp_size / 40;
            nvr_struct.vertex_lists[i].type = 2;
        } else if (temp_size / 36 * 36 == temp_size && zeros_all) {
            nvr_struct.vertex_lists[i].size = temp_size / 36;
            nvr_struct.vertex_lists[i].type = 1;
        } else {
            nvr_struct.vertex_lists[i].size = temp_size / 12;
            nvr_struct.vertex_lists[i].type = 0;
        }
        cout << "\tType: " << nvr_struct.vertex_lists[i].type << " Size: " << nvr_struct.vertex_lists[i].size << "\n";
        

        nvr_struct.vertex_lists[i].vertices = new nvr_vertex[nvr_struct.vertex_lists[i].size];

        cout << "\tIdx: " << i << "\n\n";
        float p0, p1, p2;
        if (nvr_struct.vertex_lists[i].type > 0) {
            for (int j = 0; j < nvr_struct.vertex_lists[i].size; j++) {
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[0] = _MemToFloat2(buffer_4); //x position of current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[1] = _MemToFloat2(buffer_4); //y position of current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[2] = _MemToFloat2(buffer_4); //z position of current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].normals[0] = _MemToFloat2(buffer_4); //x component of normal on current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].normals[1] = _MemToFloat2(buffer_4); //y component of normal on current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].normals[2] = _MemToFloat2(buffer_4); //z component of normal on current vertex
                _ReadNext(buffer_4, 4);
                // Known problem with UVs being infinity.
                nvr_struct.vertex_lists[i].vertices[j].uv[0] = _MemToFloat2(buffer_4); //u coordinate on texture
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].uv[1] = _MemToFloat2(buffer_4); //v coordinate on texture
                _ReadNext(buffer_4, 4); //unknown value
//                nvr_struct.vertex_lists[i].vertices[j].unknown = _MemToInt(buffer_4);
                if (nvr_struct.vertex_lists[i].type == 2) {
                    _ReadNext(buffer_4, 4);
                }
            }
        } else {
            for (int j = 0; j < nvr_struct.vertex_lists[i].size; j++) {
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[0] = _MemToFloat2(buffer_4); //x position of current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[1] = _MemToFloat2(buffer_4); //y position of current vertex
                _ReadNext(buffer_4, 4);
                nvr_struct.vertex_lists[i].vertices[j].position[2] = _MemToFloat2(buffer_4); //z position of current vertex
            }
        }
    }
    cout << "Finished }\n";

    //get index list
    _PrintStart("index_lists", nvr_struct.count_index_list);
    nvr_struct.index_lists = new nvr_indexlist_struct_class[nvr_struct.count_index_list];

    for (int i = 0; i < nvr_struct.count_index_list; i++) {
        _ReadNext(buffer_4, 4);
        nvr_struct.index_lists[i].size = _MemToInt(buffer_4) / 2; //size of current index list
        _ReadNext(buffer_4, 4);
        nvr_struct.index_lists[i].d3dfmt = _MemToShort(buffer_4); //d3dformat

        nvr_struct.index_lists[i].indices = new unsigned short[nvr_struct.index_lists[i].size];
        for (int j = 0; j < nvr_struct.index_lists[i].size; j++) {
            _ReadNext(buffer_2, 2);
            nvr_struct.index_lists[i].indices[j] = _MemToShort(buffer_2); //vertex id
        }
    }
    cout << "Finished }\n";

    //get models
    _PrintStart("models", nvr_struct.count_model);
    nvr_struct.models = new nvr_model_struct_class[nvr_struct.count_model];

    for (int i = 0; i < nvr_struct.count_model; i++) {
        _ReadNext(buffer_4, 4);
        nvr_struct.models[i].flag_1 = _MemToInt(buffer_4); //flag 1
        _ReadNext(buffer_4, 4);
        nvr_struct.models[i].flag_2 = _MemToInt(buffer_4); //flag 2
        for (int j = 0; j < 10; j++) {
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].b[j] = _MemToFloat(buffer_4); //b
        }
        _ReadNext(buffer_4, 4);
        nvr_struct.models[i].material = _MemToInt(buffer_4); //material
        for (int j = 0; j < 2; j++) {
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].model[j].vertex_index = _MemToInt(buffer_4); //vertex index
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].model[j].vertex_offset = _MemToInt(buffer_4); //vertex offset
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].model[j].vertex_length = _MemToInt(buffer_4); //vertex length
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].model[j].index_index = _MemToInt(buffer_4); //index index
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].model[j].index_offset = _MemToInt(buffer_4); //index offset
            _ReadNext(buffer_4, 4);
            nvr_struct.models[i].model[j].index_length = _MemToInt(buffer_4); //index length
        }

    }
    cout << "Finished }\n";

    //get unknown_3s
    _PrintStart("unknown_3s", nvr_struct.count_unknown_3);
    nvr_struct.unknown_3 = new nvr_unknown3_struct_class[nvr_struct.count_unknown_3];

    for (int i = 0; i < nvr_struct.count_unknown_3; i++) {
        for (int j = 0; j < 6; j++) {
            _ReadNext(buffer_4, 4);
            nvr_struct.unknown_3[i].a[j] = _MemToFloat(buffer_4); //a
        }
        for (int j = 0; j < 4; j++) {
            _ReadNext(buffer_4, 4);
            nvr_struct.unknown_3[i].b[j] = _MemToInt(buffer_4); //b
        }
    }

    cout << "Finished }\n\n\n";

    cout << "Converting " << nvr_struct.count_model << " models to OBJ Format...\n\n";

    for (int i = 0; i < nvr_struct.count_model; i++) {
        _SaveData(nvr_struct, i);
    }
    return 0;
}

void check_name(string name) {
    //    if (regex_search(name.begin(), name.end(), rx)) {
    //        cout << "FOUND A MATERIAL!!!! :>" << name << "\n";
    //    }
}

void _PrintStart(string name, int count) {
    stringstream ss;
    ss << name << "[" << count << "]";

    printf("  %-40s{ Processing... ", ss.str().c_str());
}

void _ReadNext(char *buffer, int bytes, bool seek) {

    if (!seek) {
        for (int i = 0; i < bytes; i++) {
            buffer[i] = nvr_data[i + pos];
        }
    }

    pos += bytes;
    return;
}

string _MemToString(char *buffer) {
    // This method may cause segfaults. I hope that it doesn't.
    string ret = buffer;
    return ret;
}

unsigned short _MemToShort(char *buffer) {
    return *((short*) buffer);
}

unsigned int _MemToInt(char *buffer) {
    return *((int*) buffer);
}

float _MemToFloat2(char *buffer) {
    //    int full = ((0xFF000000 & (buffer[3] << 24)) | (0x00FF0000 & (buffer[2] << 16)) | (0x0000FF00 & (buffer[1] << 8)) | (0x000000FF & buffer[0]));
    //    int sign = 0x00000001 & (full >> 31);
    //    int exponent = 0x000000FF & (full >> 23);
    //    int mantissa = 0x007FFFFF & full;
    //    float ret = round(pow(-1.0f, sign)*(1.0 + ((float) mantissa) / 8388608) * pow(2.0f, exponent - 127), 6);
    //
    //    if (!isfinite(ret)) {
    //        ret = 0;
    //    } //catch NaN and infinite
    //    return ret;
    float f = _MemToFloat(buffer);
    if (_CFR(f)) {
        cout << "Failed for " << pos - 4 << " next bytes" << f << "\n";
        cout << "Bytes: " << ((int) buffer[0]) << "|" << ((int) buffer[1]) << "|" << ((int) buffer[2]) << "|" << ((int) buffer[3]) << "|\n";
        print_data_r(buffer, 4);
        //_ReadNext(buffer, 4);
        f = _MemToFloat(buffer);
        if (_CFR(f)) {
            return 4.20420;
        }
    }
    return f;
}

float _MemToFloat(char *buffer) {
    return *((float*) buffer);
}

float round(float r, int places) {
    return floor(pow(10.0f, places) * r + 0.5f) / pow(10.0f, places);
}

string _GenHeader() {
    stringstream ss;

    ss << "#################################################################\n";
    ss << "#                                                               #\n";
    ss << "# This file is generated by LoL Map Converter as part of the    #\n";
    ss << "#   DotL project. For more information and to contribute see:   #\n";
    ss << "#                                                               #\n";
    ss << "#   https://github.com/coldAmphibian/DotL-Hammer                #\n";
    ss << "#                                                               #\n";
    ss << "#################################################################\n";
    ss << "\n\n\n";

    return ss.str();
}

void _SaveData(nvr_struct_class nvr_struct, int i) {
<<<<<<< HEAD
	int XFile_vertex_index = nvr_struct.models[i].model[0].vertex_index;
	int XFile_vertex_offset = nvr_struct.models[i].model[0].vertex_offset;
	int XFile_vertex_length = nvr_struct.models[i].model[0].vertex_length;

	int XFile_index_index = nvr_struct.models[i].model[0].index_index;
	int XFile_index_offset = nvr_struct.models[i].model[0].index_offset;
	int XFile_index_length = nvr_struct.models[i].model[0].index_length;

	string XFile_texture = nvr_struct.materials[nvr_struct.models[i].material].texture_filename;

	stringstream obj_file_name_ss;
	obj_file_name_ss
		<< nvr_struct.materials[nvr_struct.models[i].material].name
		<< nvr_struct.materials[nvr_struct.models[i].material].used
		<< ".obj";
	string obj_file_name = obj_file_name_ss.str();

	stringstream mtl_file_name_ss;
	mtl_file_name_ss
		<< nvr_struct.materials[nvr_struct.models[i].material].name
		<< nvr_struct.materials[nvr_struct.models[i].material].used
		<< ".mtl";
	string mtl_name = mtl_file_name_ss.str();


	nvr_struct.materials[nvr_struct.models[i].material].used += 1;

	cout << "Converting Model " << i << "/" << nvr_struct.count_model << "          " << obj_file_name << "\n";


	stringstream ssXFile;
	// Sets Float precision
	ssXFile << fixed << showpoint << setprecision(6);

	ssXFile << "mtllib " << mtl_name;
	ssXFile << "usemtl " << nvr_struct.materials[nvr_struct.models[i].material].name;

	ssXFile << "# List of vertices\n";

	for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
		nvr_vertex v = nvr_struct.vertex_lists[XFile_vertex_index].vertices[j];
		ssXFile << "v " << v.position[0] << " "
			<< v.position[1] << " "
			<< v.position[2] << "\n";
	}
	ssXFile << "# List of vertex normals\n";
	for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
		ssXFile << "vn " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[0] << " "
			<< nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[1] << " "
			<< nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[2] << "\n";
	}
	//texture coordinates
	for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
		ssXFile << "vt " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[0] << " "
			<< nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[1] << "\n";
	}
	//indices for positions

	//starts counting how many points are defined... IDK why I called it facecount but it fixes problems
	int faceCount = 0;

	for (int j = XFile_index_offset; j < XFile_index_offset + XFile_index_length; j += 3) {
		// F**k you 1 indexing.
		ssXFile << "f " << (1 + nvr_struct.index_lists[XFile_index_index].indices[j] - XFile_vertex_offset) << "/" << to_string(++faceCount) << " "
			<< (1 + nvr_struct.index_lists[XFile_index_index].indices[j + 1] - XFile_vertex_offset) << "/" << to_string(++faceCount) << " "
			<< (1 + nvr_struct.index_lists[XFile_index_index].indices[j + 2] - XFile_vertex_offset) << "/" << to_string(++faceCount) << "\n";
	}

	//save to file
	ofstream XFile;
	XFile.open(obj_file_name);
	XFile << _GenHeader() << ssXFile.str();
	XFile.close();
	XFile.clear();

	//        ssXFile.clear()
	//        ssXFile << "newmtl " << "";
	//               
	XFile.open(mtl_name);
	XFile << _GenHeader()
		<< "newmtl " << XFile_texture << "\n"
		<< "Ka 0.000000 0.000000 0.000000\n"
		<< "Kd 1.000000 1.000000 1.000000\n"
		<< "Ks 0.000000 0.000000 0.000000\n"
		<< "d 1.000000\n"
		<< "illum 2\n"
		<< "Ns 3.437500\n";
	XFile.close();
=======
    int XFile_vertex_index = nvr_struct.models[i].model[0].vertex_index;
    int XFile_vertex_offset = nvr_struct.models[i].model[0].vertex_offset;
    int XFile_vertex_length = nvr_struct.models[i].model[0].vertex_length;

    int XFile_index_index = nvr_struct.models[i].model[0].index_index;
    int XFile_index_offset = nvr_struct.models[i].model[0].index_offset;
    int XFile_index_length = nvr_struct.models[i].model[0].index_length;

    string XFile_texture = nvr_struct.materials[nvr_struct.models[i].material].texture_filename;

    stringstream obj_file_name_ss;
    obj_file_name_ss
            << nvr_struct.materials[nvr_struct.models[i].material].name
            << nvr_struct.materials[nvr_struct.models[i].material].used
            << ".obj";
    string obj_file_name = obj_file_name_ss.str();

    stringstream mtl_file_name_ss;
    mtl_file_name_ss
            << nvr_struct.materials[nvr_struct.models[i].material].name
            << nvr_struct.materials[nvr_struct.models[i].material].used
            << ".mtl";
    string mtl_name = mtl_file_name_ss.str();


    nvr_struct.materials[nvr_struct.models[i].material].used += 1;

    cout << "Converting Model " << i << "/" << nvr_struct.count_model << "          " << obj_file_name << "\n";

    cout << "\tIdx: " << XFile_vertex_index << " Off:" << XFile_vertex_offset << " Len:" << XFile_vertex_length << "\n";
    

    stringstream ssXFile;
    // Sets Float precision
    ssXFile << fixed << showpoint << setprecision(6);

    ssXFile << "mtllib " << mtl_name << "\n";

    ssXFile << "\n\n# Vertices: " << XFile_vertex_length << "\n";

    for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
        nvr_vertex v = nvr_struct.vertex_lists[XFile_vertex_index].vertices[j];
        ssXFile << "v " << v.position[0] << " "
                << v.position[1] << " "
                << v.position[2] << "\n";
    }
    ssXFile << "# End Vertices: " << XFile_vertex_length << "\n";

    ssXFile << "\n\n# Vertex Normals: " << XFile_index_length << "\n";
    for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
        ssXFile << "vn " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[0] << " "
                << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[1] << " "
                << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].normals[2] << "\n";
    }
    ssXFile << "# End Vertex Normals: " << XFile_index_length << "\n";
    //texture coordinates

    ssXFile << "\n\n# UV: " << XFile_index_length << "\n";
    for (int j = XFile_vertex_offset; j < XFile_vertex_offset + XFile_vertex_length; j++) {
        ssXFile << "vt " << nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[0] << " "
                << (1 - nvr_struct.vertex_lists[XFile_vertex_index].vertices[j].uv[1]) << "\n";
    }
    ssXFile << "\n\n# End UV: " << XFile_index_length << "\n";

    ssXFile << "usemtl " << nvr_struct.materials[nvr_struct.models[i].material].name << "\n";
    //indices for positions
    for (int j = XFile_index_offset; j < XFile_index_offset + XFile_index_length; j += 3) {
        // F**k you 1 indexing.
        ssXFile << "f " << (1 + nvr_struct.index_lists[XFile_index_index].indices[j] - XFile_vertex_offset) << " "
                << (1 + nvr_struct.index_lists[XFile_index_index].indices[j + 1] - XFile_vertex_offset) << " "
                << (1 + nvr_struct.index_lists[XFile_index_index].indices[j + 2] - XFile_vertex_offset) << "\n";
    }

    //save to file
    ofstream XFile;
    XFile.open(obj_file_name);
    XFile << _GenHeader() << ssXFile.str();
    XFile.close();
    XFile.clear();

    //        ssXFile.clear()
    //        ssXFile << "newmtl " << "";
    //               
    XFile.open(mtl_name);
    XFile << _GenHeader()
            << "newmtl " << XFile_texture << "\n"
            << "Ka 0.000000 0.000000 0.000000\n"
            << "Kd 1.000000 1.000000 1.000000\n"
            << "Ks 0.000000 0.000000 0.000000\n"
            << "d 1.000000\n"
            << "illum 2\n"
            << "Ns 3.437500\n";
    XFile.close();
>>>>>>> 7922d418df94767f4d5f1fa48cdd3791adeefde7
}