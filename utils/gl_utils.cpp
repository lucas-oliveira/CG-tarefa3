/* support programming
\******************************************************************************/
#include "gl_utils.h"


bool parse_file_into_str( const char *file_name, char *shader_str, int max_len ) {
	FILE *file = fopen( file_name, "r" );
	if ( !file ) {
		return false;
	}
	size_t cnt = fread( shader_str, 1, max_len - 1, file );
	if ( (int)cnt >= max_len - 1 ) {
	}
	if ( ferror( file ) ) {
		fclose( file );
		return false;
	}
	// append \0 to end of file string
	shader_str[cnt] = 0;
	fclose( file );
	return true;
}
