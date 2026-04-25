#include <stdio.h>
#include <stdlib.h>

#include "bun.h"

static void print_header_summary(const BunHeader *header) {
  printf("BUN Header\n");
  printf("  magic: 0x%08X\n", header->magic);
  printf("  version: %u.%u\n",
    (unsigned)header->version_major,
    (unsigned)header->version_minor);
  printf("  asset_count: %u\n", (unsigned)header->asset_count);
  printf("  asset_table_offset: %llu\n",
    (unsigned long long)header->asset_table_offset);
  printf("  string_table_offset: %llu\n",
    (unsigned long long)header->string_table_offset);
  printf("  string_table_size: %llu\n",
    (unsigned long long)header->string_table_size);
  printf("  data_section_offset: %llu\n",
    (unsigned long long)header->data_section_offset);
  printf("  data_section_size: %llu\n",
    (unsigned long long)header->data_section_size);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file.bun>\n", argv[0]);
    return BUN_ERR_IO;
  }
  const char *path = argv[1];

  BunParseContext ctx = {0};
  BunHeader header  = {0};

  bun_result_t result = bun_open(path, &ctx);
  if (result != BUN_OK) {
    fprintf(stderr, "Error: could not open '%s'\n", path);
    return result;
  }

  result = bun_parse_header(&ctx, &header);
  if (result != BUN_OK) {
    // Implementation 8: Human-readable output for invalid outcomes.
    if (result == BUN_MALFORMED) {
      fprintf(stderr, "Violation: header is malformed\n");
    } else if (result == BUN_UNSUPPORTED) {
      fprintf(stderr, "Violation: unsupported BUN version or feature in header\n");
    } else {
      fprintf(stderr, "Error: failed to parse header (code %d)\n", result);
    }
    bun_close(&ctx);
    return result;
  }

  // Implementation 8: Human-readable output for valid outcomes.
  print_header_summary(&header);

  result = bun_parse_assets(&ctx, &header);
  if (result == BUN_MALFORMED) {
    fprintf(stderr, "Violation: one or more asset records are malformed\n");
  } else if (result == BUN_UNSUPPORTED) {
    fprintf(stderr, "Violation: one or more asset records use unsupported features\n");
  } else if (result != BUN_OK) {
    fprintf(stderr, "Error: failed while parsing asset records (code %d)\n", result);
  }

  bun_close(&ctx);
  return result;
}
