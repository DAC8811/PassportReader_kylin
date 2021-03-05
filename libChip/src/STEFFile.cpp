/*
 * STEFFile.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: echo
 */

//#include <iostream>
#include <string>
#include <map>
#include "Ptypes.h"
#include "STEFFile.h"
#include "EFFile.h"

#if USE_OPENJPEG

#include <openjpeg-2.3/openjpeg.h>
#include "JP2.h"

#endif

void EF_Default_File_Init(STEFFile *file) {

    file->Tag = 0;
    file->Index = EF_UNKNOWN;
    file->name[0] = '\0';
    file->Id[0] = '\0';

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void EF_COM_File_Init(STEFFile *file) {
    file->Tag = 0x60;
    file->Index = EF_COM;
    file->Id[0] = 0x01;
    file->Id[1] = 0x1E;
    file->Id[2] = '\0';
    memcpy(file->name, "EF.COM", sizeof("EF.COM"));
    file->name[sizeof("EF.COM")] = '\0';

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void EF_DG1_File_Init(STEFFile *file) {
    file->Tag = 0x61;
    file->Index = EF_DG1;
    file->Id[0] = 0x01;
    file->Id[1] = 0x01;
    file->Id[2] = '\0';
    memcpy(file->name, "EF.DG1", sizeof("EF.DG1"));
    file->name[sizeof("EF.DG1")] = '\0';

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void EF_DG2_File_Init(STEFFile *file) {
    file->Tag = 0x75;
    file->Index = EF_DG2;
    file->Id[0] = 0x01;
    file->Id[1] = 0x02;
    file->Id[2] = '\0';
    memcpy(file->name, "EF.DG2", sizeof("EF.DG2"));
    file->name[sizeof("EF.DG2")] = '\0';

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void EF_DG11_File_Init(STEFFile *file) {
    file->Tag = 0x6b;
    file->Index = EF_DG11;
    file->Id[0] = 0x01;
    file->Id[1] = 0x0B;
    file->Id[2] = '\0';
    memcpy(file->name, "EF.DG11", sizeof("EF.DG11"));
    file->name[sizeof("EF.DG11")] = '\0';

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void EF_IDINFO_File_Init(STEFFile *file) {
    file->Tag = 0;
    file->Index = EF_IDINFO;
    file->Id[0] = '\0';
    memcpy(file->name, "EF.IDINFO", sizeof("EF.IDINFO"));

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void EF_IDPIC_File_Init(STEFFile *file) {
    file->Tag = 0;
    file->Index = EF_IDPIC;
    file->Id[0] = '\0';
    memcpy(file->name, "EF.IDPIC", sizeof("EF.IDPIC"));
    file->name[sizeof("EF.IDPIC")] = '\0';

    file->resultLen = 0;
    file->result[0] = '\0';
    file->resultPath[0] = '\0';

    file->FileParse = NULL;
    file->Valid = NULL;
}

void STEFilesInit(STEFFileSystem *fileSystem) {

    EF_COM_File_Init(&(fileSystem->stEFFiles[0]));
    EF_DG1_File_Init(&(fileSystem->stEFFiles[1]));
    EF_DG2_File_Init(&(fileSystem->stEFFiles[2]));
    EF_DG11_File_Init(&(fileSystem->stEFFiles[11]));
    EF_IDINFO_File_Init(&(fileSystem->stEFFiles[18]));
    EF_IDPIC_File_Init(&(fileSystem->stEFFiles[19]));
    fileSystem->count = 5;

    for (int i = 3; i < MAX_ST_EFFILE; i++) {
        if (i == 18 || i == 19 || i == 11) {
            continue;
        }
        EF_Default_File_Init(&(fileSystem->stEFFiles[i]));
    }
}

STEFFile *StIndexFindEFFile(EF_NAME name, STEFFileSystem *fileSystem) {
    return &(fileSystem->stEFFiles[name]);
}

STEFFile *StTagFindEFFile(unsigned char tag, STEFFileSystem *fileSystem) {
    for (int i = 0; i < MAX_ST_EFFILE - 1; i++) {
        if (tag == fileSystem->stEFFiles[i].Tag) {
            return &(fileSystem->stEFFiles[i]);
        }
    }

    return &(fileSystem->stEFFiles[MAX_ST_EFFILE - 1]);
}

