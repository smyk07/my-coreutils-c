#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arguments {
  unsigned int file_count;
  char **files;
} arguments;

void parse_arguments(int argc, char **argv, arguments *args) {
  args->files = malloc(argc * sizeof(char *));

  if (args->files == NULL) {
    perror("malloc failed");
    exit(1);
  }

  int index = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      printf("Usage: ./cat [FILES] [--help]\n");
      printf("Concatenate FILE(s) to stdout.\n");
      printf("If no FILE, read stdin.\n");
      free(args->files);
      exit(0);
    } else {
      args->files[index] = argv[i];
      index += 1;
    }
  }

  args->file_count = index;
}

#define MAX_LEN 4096

int read_file(char *path, char **buffer) {
  int tmp_capacity = MAX_LEN;
  char *tmp = malloc(tmp_capacity * sizeof(char));

  if (tmp == NULL) {
    perror("malloc failed");
    exit(1);
  }

  int tmp_size = 0;

  // FILE *f = fopen(path, "r");
  FILE *f = fopen(path, "r");

  if (f == NULL) {
    perror("fopen failed");
    free(tmp);
    exit(1);
  }

  int size = 0;

  do {
    if (tmp_size + MAX_LEN >= tmp_capacity) {
      tmp_capacity *= 2;
      tmp = realloc(tmp, tmp_capacity * sizeof(char));

      if (tmp == NULL) {
        perror("realloc failed");
        free(tmp);
        fclose(f);
        exit(1);
      }
    }

    size = fread(tmp + tmp_size, sizeof(char), MAX_LEN, f);
    tmp_size += size;
  } while (size > 0);

  fclose(f);

  *buffer = tmp;

  return tmp_size;
}

int read_stdin(char **buffer) {
  int tmp_capacity = MAX_LEN;
  char *tmp = malloc(tmp_capacity * sizeof(char));

  if (tmp == NULL) {
    perror("malloc failed");
    exit(1);
  }

  int tmp_size = 0;
  int size = 0;

  while ((size = fread(tmp + tmp_size, sizeof(char), MAX_LEN, stdin)) > 0) {
    tmp_size += size;
    if (tmp_size + MAX_LEN >= tmp_capacity) {
      tmp_capacity *= 2;
      tmp = realloc(tmp, tmp_capacity * sizeof(char));

      if (tmp == NULL) {
        perror("realloc failed");
        free(tmp);
        exit(1);
      }
    }
  }

  *buffer = tmp;

  return tmp_size;
}

int main(int argc, char **argv) {
  arguments args = {0};
  parse_arguments(argc, argv, &args);

  char *buffer = NULL;
  int buffer_size = 0;

  if (args.file_count == 0) {
    char *content = NULL;
    int size = read_stdin(&content);

    if (size > 0)
      fwrite(content, sizeof(char), size, stdout);
    free(content);
  } else {
    for (int i = 0; i < args.file_count; i++) {
      char *content = NULL;
      int size = read_file(args.files[i], &content);

      buffer = realloc(buffer, buffer_size + size);

      if (buffer == NULL) {
        perror("realloc failed");
        free(content);
        free(buffer);
        free(args.files);
        exit(1);
      }

      memcpy(buffer + buffer_size, content, size);
      buffer_size += size;

      free(content);
    }

    if (buffer_size > 0) {
      fwrite(buffer, sizeof(char), buffer_size, stdout);
    }
  }

  free(buffer);
  free(args.files);

  return 0;
}
