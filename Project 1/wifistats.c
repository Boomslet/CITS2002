/*
   CITS2002 Project 1 2017
   Name(s):             Mark Boon
   Student number(s):   21750965
   Date:                21/09/2017
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_ADDRESSES 500   // 500 addresses
#define ADDR_SIZE 17        // 17 chars per address
#define OUI_LENGTH 8        // 8 chars per OUI
#define LONGEST_VENDOR 90   // 90 chars in longest vendor name
#define LONGEST_INT 10      // 10 digit maximum length in 32-bit integer


// holds unique device data
char device_list[MAX_ADDRESSES][OUI_LENGTH + 1 + LONGEST_VENDOR + 1];

// holds packet data for unique each device
int packets[MAX_ADDRESSES];


/**
* Function: sort_results
* ----------------------------
* @brief Sorts each element of the results array in descending
* order of packets by calling the sort() system call
*
* @param void
*
* @return void
*/

void sort_results(void)
{
  int pid;
  int status;

  char *args[] = {"/usr/bin/sort",
                  "-t\t",     // delimit by '\t'
                  "-k2,2rn",  // sort column 2 reverse numerical
                  "-k3,3rn",  // sort column 3 reverse numerical
                  "-k1,1",    // default sort column 1
                  "report.txt",
                  NULL};

  switch (pid = fork())
  {
    case -1:
      perror("fork()");
      exit(EXIT_FAILURE);
      break;

    case 0:
      if ((execv(args[0], args)) == -1)
      {
        perror("execv()");
      }
      break;

    default:
      wait(&status);
      break;
  }
}


/**
* Function: write_report
* ----------------------------
* @brief Combines unknown data from the device_list and
* packets arrays if present, then prints data pairs from
* device_list and packets to each line of a text file
*
* @param length Integer length of device_list and packets arrays
* @param report_fp FILE pointer to output text file to be written
*
* @return void
*/

void write_report(FILE *report_fp, int length)
{
  int i = 0;

  int unknown_packets = 0;

  while(i < length)
    {
      // accumulate packets from flagged (unknown) devices
      if (!strcmp(device_list[i], "flagged"))
      {
        unknown_packets += packets[i];
      }

      else
      {
        fprintf(report_fp, "%s\t%d\n", device_list[i], packets[i]);
      }

      i++;
    }

    if (unknown_packets > 0)
    {
      // print any unknown packets under a single entry "UNKNOWN-VENDOR"
      fprintf(report_fp, "%s\t%d\n",
              "??:??:??\tUNKNOWN-VENDOR", unknown_packets);
    }

  fclose(report_fp);
}


/**
* Function: clean_address
* ----------------------------
* @brief Alters a given MAC address or OUI to be all lower-case
* characters seperated into hexadecimal segments by ':'
*
* @param address Character string address to be cleaned
*
* @return void
*/

void clean_address(char address[])
{
  int length = strlen(address);

  for (int i = 0; i < length; i++)
  {
    if (i % 3 == 2)
    {
      address[i] = ':';
    }

    // type casting allows tolower() on all chars in the address
    address[i] = tolower((unsigned char)address[i]);
  }
}


/**
* Function: match_oui
* ----------------------------
* @brief Modifies an element of the device_list array to reflect
* it's manufacturer name if it matches an OUI in oui_list, else
* flags an unmatched device
*
* @param oui_fp FILE pointer to oui_list
* @param length Integer length of device_list array
*
* @return void
*/

void match_oui(FILE *oui_fp, int length)
{
  // holds file data read from each line
  char manufacturer[LONGEST_VENDOR + 1];
  char file_oui[OUI_LENGTH + 1];

  char line[BUFSIZ];

  while (fgets(line, sizeof line, oui_fp) != NULL)
  {
    sscanf(line, "%s %[^\n]", file_oui, manufacturer);

    clean_address(file_oui);

    for (int i = 0; i < length; i++)
    {
      if (!strcmp(file_oui, device_list[i]))
      {
        sprintf(device_list[i], "%s\t%s", file_oui, manufacturer);
        break;
      }
    }
  }

  for (int j = 0; j < length; j++)
  {
    if (strlen(device_list[j]) == OUI_LENGTH)
    {
      sprintf(device_list[j], "flagged");
    }
  }

  fclose(oui_fp);
}


/**
* Function: check_exists
* ----------------------------
* @brief Checks if a device ID already exists in the device_list array
*
* @param dev_id Character string representing the device ID to be tested
* @param length Integer length of device_list array's current size
*
* @returns exists Integer index of device_list array
* where device_id matches an element, else returns -1
*/

int check_exists(char device_id[], int length)
{
  int exists = -1;

  for (int i = 0; i < length; i++)
  {
    if (!strcmp(device_id, device_list[i]))
    {
      exists = i;
      break;
    }
  }

  return exists;
}


/**
* Function: count_packets
* ----------------------------
* @brief Populates the device_list and packets arrays
* with data read from wifi_data text file
*
* @param mode Integer representing in which mode count_packets should operate
* @param wifi_data_fp FILE pointer to wifi_data text file
* @param oui_file_exists Boolean representing if an oui file
* has been supplied to the program
*
* @return i Integer length of the device_list (and packets) array
*/

int count_packets(int mode, FILE *wifi_data_fp, bool oui_file_exists)
{
  // holds file data read from each line
  char transmitter[ADDR_SIZE + 1];
  char receiver[ADDR_SIZE + 1];
  int packet_count;

  // determines which device data to capture, according to mode
  char *device = transmitter;

  if (mode == 'r')
  {
    device = receiver;
  }

  // determines where to terminate device string,
  // depending on the presence of an OUI file
  int nul_index = ADDR_SIZE;

  if (oui_file_exists)
  {
    nul_index = OUI_LENGTH;
  }

  char line[BUFSIZ];
  int i = 0;

  // will hold the return value from check_exists function
  int index;

  while (fgets(line, sizeof line, wifi_data_fp) != NULL)
  {
    sscanf(line, "%*s %s %s %i", transmitter, receiver, &packet_count);

    clean_address(device);

    if (!strcmp(receiver, "ff:ff:ff:ff:ff:ff"))
    {
      continue;
    }

    // terminate device string at nul_index
    device[nul_index] = '\0';

    index = check_exists(device, i);

    if (index != -1)
    {
      // increment packets at index and return to loop header
      // to preserve i as a correct index
      packets[index] += packet_count;
    }

    else
    {
      sprintf(device_list[i], "%s", device);

      packets[i] = packet_count;

      i++;
    }
  }

  fclose(wifi_data_fp);

  return i;
}


/**
* Function: open_file
* ----------------------------
* @brief Attempts to open a text file for reading
*
* @param filename Character array file name of text file to open
* @param access_mode Character flag representing file access mode
*
* @return FILE pointer to opened file, in_file
*/

FILE *open_file(char filename[], char access_mode[])
{
  FILE *in_file;
  in_file = fopen(filename, access_mode);

  if (in_file == NULL)
  {
    perror(filename);
    exit(EXIT_FAILURE);
  }

  return in_file;
}


/**
* Function: main
* ----------------------------
* @brief Reports the number of bytes either transmitted or received
* by devices using wifi. May generate reports by MAC address or by
* vendor name. Sorted from greatest number of packets to smallest.
*
* @param argc Integer number of command-line arguments
* @param argv[0] Character representing program mode of operation,
* must be either 'r' or 't'
* @param argv[1] Character array representing the name of a text file
* containing wifi device data
* @param argv[2] Optional character array argument representing the name
* of a text file containing OUI and vendor name information
*
* @return status Integer exit status
*/

int main(int argc, char *argv[])
{
  int status = EXIT_SUCCESS;

  // checks for correct number of args
  if (argc < 3 || argc > 4)
  {
    fprintf(stderr, "%s: expected 2 or 3 arguments, received %d\n",
            argv[0], argc-1);

    status = EXIT_FAILURE;
  }

  // checks if first argument supplied is valid
  else if (strlen(argv[1]) > 1 || (argv[1][0] != 't' && argv[1][0] != 'r'))
  {
    fprintf(stderr, "%s: invalid argument: %s", argv[0], argv[1]);

    status = EXIT_FAILURE;
  }

  else
  {
    bool oui_file_exists = (argc == 4);

    FILE *wifi_data = open_file(argv[2], "r");

    int array_length = count_packets(argv[1][0], wifi_data, oui_file_exists);

    if (oui_file_exists)
    {
      FILE *oui_list = open_file(argv[3], "r");

      match_oui(oui_list, array_length);
    }

    FILE *report = open_file("report.txt", "w+");

    write_report(report, array_length);

    sort_results();
  }

  return status;
}
