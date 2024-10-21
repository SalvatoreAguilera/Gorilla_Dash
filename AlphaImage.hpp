class AlphaImage {
	public:
	int width, height, channels;
	unsigned char *data;

	AlphaImage(const char* filename);
};