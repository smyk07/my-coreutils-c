#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *argv_to_string(int argc, char **argv, int slice_from, int newline) {
  int length = argc - 1;
  for (int i = slice_from; i < argc; i++) {
    length += strlen(argv[i]) + 1;
  }

  char *result = malloc(length + 1);

  if (result == NULL) {
    perror("malloc failed");
    exit(1);
  }

  result[0] = '\0';
  for (int i = slice_from; i < argc; i++) {
    strcat(result, argv[i]);
    if (i < argc - 1)
      strcat(result, " ");
  }

  if (newline)
    strcat(result, "\n");
  else
    strcat(result, "\0");

  return result;
}

char *process_escape_sequences(char *buffer) {
  int length = strlen(buffer);
  char *output = malloc(length + 1);

  if (output == NULL) {
    perror("malloc failed");
    free(buffer);
    exit(1);
  }

  int j = 0;
  for (int i = 0; i < length; i++) {
    if (buffer[i] == '\\' && i + 1 < length) {
      switch (buffer[i + 1]) {
      case 'n':
        output[j++] = '\n';
        break;
      case 't':
        output[j++] = '\t';
        break;
      case 'a':
        output[j++] = '\a';
        break;
      case 'b':
        output[j++] = '\b';
        break;
      case 'r':
        output[j++] = '\r';
        break;
      case 'f':
        output[j++] = '\f';
        break;
      case 'v':
        output[j++] = '\v';
        break;
      case '\\':
        output[j++] = '\\';
        break;
      case '\"':
        output[j++] = '\"';
        break;
      default:
        output[j++] = buffer[i];
        output[j++] = buffer[i + 1];
      }
      i++;
    } else {
      output[j++] = buffer[i];
    }
  }

  output[j] = '\0';

  return output;
}

void parse_arguments(int argc, char **argv, char **buffer) {
  if (argc > 1) {
    if (strcmp(argv[1], "--help") == 0) {
      printf("./echo [STRING] [--help]\n");
      printf("Display a line of text.\n");
      exit(0);
    } else if (strcmp(argv[1], "-e") == 0) {
      char *raw_string = argv_to_string(argc, argv, 2, 1);
      *buffer = process_escape_sequences(raw_string);
      free(raw_string);
    } else if (strcmp(argv[1], "-E") == 0) {
      *buffer = argv_to_string(argc, argv, 2, 1);
    } else if (strcmp(argv[1], "-n") == 0) {
      *buffer = argv_to_string(argc, argv, 2, 0);
    } else {
      *buffer = argv_to_string(argc, argv, 1, 1);
    }
  } else {
    *buffer = strdup("\n");
  }
}

int main(int argc, char **argv) {
  char *content = NULL;
  parse_arguments(argc, argv, &content);

  fwrite(content, sizeof(char), strlen(content), stdout);
  free(content);

  return 0;
}
