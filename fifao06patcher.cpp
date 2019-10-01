#include <iostream>
#include <limits>
#include <fstream>

using namespace std;

#define byte unsigned char

uint checksumCalculation(uint *buffer, int size)
{
  uint *ptrToBuffer;
  uint tmpByte;
  uint checksum;
  
  checksum = 0x15; //initial "seed" value

  if (size != 0) {
    do {
      ptrToBuffer = (uint*)buffer;
      if (size < 4) {
        do {
          checksum = (uint)*(byte *)ptrToBuffer * size + checksum & 0xffff;
	  size = size + -1;
          ptrToBuffer = (uint *)((int)ptrToBuffer + 1);
        } while (size != 0);
      }
      else {
        tmpByte = *buffer;
        checksum = (tmpByte >> 0x18) * size +
                   ((tmpByte >> 0x10 & 0xff) * size +
                    ((tmpByte >> 8 & 0xff) * size + ((tmpByte & 0xff) * size + checksum & 0xffff) &
                    0xffff) & 0xffff) & 0xffff;
	//cout << "CRC: " << hex << checksum << endl;
        size = size - 4;
        buffer = buffer + 1; //in theory, +1 would be 4 bytes here. Pointer arithmetic. 
}
    } while (size != 0);
  }
  return checksum;
}

int main()
{
	string filename;
	cout << "fifa06 patcher" << endl;
	cout << "input file to patch: "; cin >> filename;

	fstream file(filename, ios::binary | ios::in | ios::out);

	if(file.is_open())
	{

		//get size of file
		file.ignore( std::numeric_limits<std::streamsize>::max() );
		std::streamsize length = file.gcount();
		file.clear();   //  Since ignore will have set eof.
		file.seekg( 0, std::ios_base::beg );

		//read file into memory
		char *memory = new char[length];
		file.read(memory, length);


		//patch crc
		//r0= 0x02243654  , r1=0x124
		//0x02243654 correspond to byte 52 of file mapped into memory
		cout << "[>] Computing checksum.." << endl;
		int checksum = checksumCalculation((uint*)(memory+52),0x124);
		cout << endl << "[>] Estimated CRC: 0x" << hex << checksum << endl;
		
		cout << "[>] Patching.." << endl;
		file.seekg(0xA);//Checksum address
		file.write(reinterpret_cast<const char *>(&checksum), sizeof(checksum));		

		file.close();
		delete [] memory;
	}
	else
	{
		cout << "error opening file" << endl;
		exit(1);
	}
	return 0;
}
