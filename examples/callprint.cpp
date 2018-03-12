typedef struct {
	long long length;
	char* data;
}string;
extern "C" void __stdcall print_string(string s);
int main() {
	string s;
	s.length = 6;
	s.data = "hello";
	print_string(s);
	return 0;
}
