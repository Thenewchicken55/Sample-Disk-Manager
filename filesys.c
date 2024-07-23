// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:_____________________________________ Date:_____today________

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <string.h>

#define TOTAL_SECTORS 511
#define SECTOR_SIZE 512

int main(int argc, char* argv[])
{
    int i, j, size, noSecs, startPos, numFiles = 0;
    int totalUsedSpace = 0;

    //open the floppy image
    FILE* floppy;
    floppy=fopen("floppya.img","r+");
    if (floppy==0)
    {
        printf("floppya.img not found\n");
        return 0;
    }

    //load the disk map from sector 256
    char map[512];
    fseek(floppy,512*256,SEEK_SET);
    for(i=0; i<512; i++)
        map[i]=fgetc(floppy);

    //load the directory from sector 257
    char dir[512];
    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++)
        dir[i]=fgetc(floppy);

    if (argc > 1 && strcmp(argv[1], "L") == 0) {
        // print directory
        printf("\nDisk directory:\n");
        printf("Name    Length\n");
        for (i=0; i<512; i=i+16) {
            if (dir[i]==0) break;
            ++numFiles;

            // print file names
            for (j=0; j<8; j++) {
                if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
            }

            // print file size
            printf(" %6d bytes\n", 512*dir[i+10]);
            totalUsedSpace += 512*dir[i+10];
        }

        // print total used space and free space
        printf("\n");
        printf("\n%d File(s)              %d bytes\n", numFiles, totalUsedSpace);
        printf("                     %d bytes free\n", TOTAL_SECTORS * SECTOR_SIZE - totalUsedSpace);
    }
    else if (argc > 1 && strcmp(argv[1], "P") == 0) {
        // check if file name is provided
        if (argc < 3) {
            printf("Please provide a file name.\n");
            return 0;
        }

        // get the file name without extension
        char* fileName = argv[2];
        char fileWithoutExtension[9];
        strncpy(fileWithoutExtension, fileName, 8);
        fileWithoutExtension[8] = '\0';

        // search for the file in the directory
        int fileIndex = -1;
        for (i = 0; i < 512; i += 16) {
            if (dir[i] == 0) break;
            char file[9];
            strncpy(file, dir + i, 8);
            file[8] = '\0';

            // if found the file, store the index
            if (strcmp(file, fileWithoutExtension) == 0) {
                fileIndex = i;
                break;
            }
        }

        // check if file is found
        if (fileIndex == -1) {
            printf("File not found.\n");
            return 0;
        }

        // check if file is executable
        if (dir[fileIndex + 8] == 'x' || dir[fileIndex + 8] == 'X') {
            printf("Cannot print executable file.\n");
            return 0;
        }

        // get the starting sector and sector count of the file
        int startSector = dir[fileIndex + 9];
        int sectorCount = dir[fileIndex + 10];

        // load the file into a buffer
        char buffer[12288];
        fseek(floppy, 512 * startSector, SEEK_SET);
        for (i = 0; i < sectorCount * SECTOR_SIZE; i++) {
            buffer[i] = fgetc(floppy);
        }

        // print the file
        printf("\nFile content:\n");
        for (i = 0; i < sectorCount * SECTOR_SIZE; i++) {
            if (buffer[i] == 0) break;
            printf("%c", buffer[i]);
        }
        printf("\n");
    }
    else if (argc > 1 && strcmp(argv[1], "M") == 0){
        // check if option M is followed by a file name
        if (argc < 3) {
            printf("Please provide a file name.\n");
            return 0;
        }
        // get the file name without extension
        char* fileName = argv[2];
        char fileWithoutExtension[9];
        strncpy(fileWithoutExtension, fileName, 8);
        fileWithoutExtension[8] = '\0';

        // check if file name is valid
        if (strlen(fileWithoutExtension) == 0) {
            printf("Invalid file name.\n");
            return 0;
        }

        // check if file name already exists
        for (i = 0; i < 512; i += 16) {
            if (dir[i] == 0) break;

            char file[9];
            strncpy(file, dir + i, 8);
            file[8] = '\0';

            if (strcmp(file, fileWithoutExtension) == 0) {
                printf("Duplicate or invalid file name.\n");
                return 0;
            }
        }

        // find a free directory entry
        int freeEntryIndex = -1;
        for (i = 0; i < 512; i += 16) {
            if (dir[i] == 0) {
                freeEntryIndex = i;
                break;
            }
        }

        // check if there is a free directory entry
        if (freeEntryIndex == -1) {
            printf("Insufficient disk space.\n");
            return 0;
        }

        // copy the file name to the directory entry
        strncpy(dir + freeEntryIndex, fileWithoutExtension, 8);

        // set the file type to "t"
        dir[freeEntryIndex + 8] = 't';

        // find a free sector on the disk
        int freeSector = -1;
        for (i = 0; i < 512; i++) {
            if (map[i] == 0) {
                freeSector = i;
                break;
            }
        }

        // check if there is a free sector
        if (freeSector == -1) {
            printf("Insufficient disk space.\n");
            return 0;
        }

        // set the map entry for the free sector to 255 (occupied)
        map[freeSector] = 255;

        // set the starting sector and length in the directory entry
        dir[freeEntryIndex + 9] = freeSector;
        dir[freeEntryIndex + 10] = 1;

        // prompt the user for a string of text
        printf("Enter the text to be stored in the file:\n");
        char text[512];
        fgets(text, sizeof(text), stdin);

        // write the file to the correct sector
        fseek(floppy, 512 * freeSector, SEEK_SET);
        fwrite(text, sizeof(char), sizeof(text), floppy);

        // write the map and directory sectors back to the disk
        fseek(floppy, 512 * 256, SEEK_SET);
        fwrite(map, sizeof(char), sizeof(map), floppy);

        fseek(floppy, 512 * 257, SEEK_SET);
        fwrite(dir, sizeof(char), sizeof(dir), floppy);

        printf("File created successfully.\n");
    }
    else if (argc > 1 && strcmp(argv[1], "D") == 0){
        // check if file name is provided
        if (argc < 3) {
            printf("Please provide a file name.\n");
            return 0;
        }

        // get the file name without extension
        char* fileName = argv[2];
        char fileWithoutExtension[9];
        strncpy(fileWithoutExtension, fileName, 8);
        fileWithoutExtension[8] = '\0';

        // search for the file in the directory
        int fileIndex = -1;
        for (i = 0; i < 512; i += 16) {
            if (dir[i] == 0) break;
            char file[9];
            strncpy(file, dir + i, 8);
            file[8] = '\0';

            if (strcmp(file, fileWithoutExtension) == 0) {
                fileIndex = i;
                break;
            }
        }

        // check if file is found
        if (fileIndex == -1) {
            printf("File not found.\n");
            return 0;
        }

        // set the first byte of the file name to zero
        dir[fileIndex] = 0;

        // get the starting sector and sector count of the file
        int startSector = dir[fileIndex + 9];
        int sectorCount = dir[fileIndex + 10];

        // set the corresponding map bytes to zero (optional)
        for (i = startSector; i < startSector + sectorCount; i++) {
            map[i] = 0;
        }

        // write the map and directory sectors back to the disk
        fseek(floppy, 512 * 256, SEEK_SET);
        fwrite(map, sizeof(char), sizeof(map), floppy);

        fseek(floppy, 512 * 257, SEEK_SET);
        fwrite(dir, sizeof(char), sizeof(dir), floppy);

        printf("File deleted successfully.\n");
    }
    else {
        printf("Invalid option.\n");
    }

    fclose(floppy);
}
