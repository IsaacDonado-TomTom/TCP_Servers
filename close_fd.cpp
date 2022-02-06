#include <unistd.h>
#include <iostream>
int	main(int argc, char** argv)
{
	if (argc == 2)
	{
		if (close(atoi(argv[1])) == 0)
		{
			std::cout << "Successfully closed file descriptor #" << atoi(argv[1]) << std::endl;
		}
	}
	return (0);
}
