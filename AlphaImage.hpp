class AlphaImage {
	public:
	int width, height, channels;
	unsigned char *data;
	~AlphaImage() { delete[] data; }
	AlphaImage(const char* filename);
};