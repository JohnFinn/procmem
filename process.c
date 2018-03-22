#include <stdio.h>
// #include <sys/types.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{

	char volatile string[] = "Hello world";

	printf("%d\n", getpid());
	printf("%p\n", string);

	while (getchar() != EOF)
		printf("%s", string);

	return 0;
}