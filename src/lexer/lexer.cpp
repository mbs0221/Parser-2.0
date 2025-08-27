#include "lexer/lexer.h"
#include "lexer/value.h"

// 词法分析器实现
Lexer::Lexer(){
	words["int"] = Type::Int;// new Word(INT, "int");
	words["double"] = Type::Double;// new Word(INT, "int");
	words["if"] = new Word(IF, "if");
	words["then"] = new Word(THEN, "then");
	words["else"] = new Word(ELSE, "else");
	words["while"] = new Word(WHILE, "while");
	words["do"] = new Word(DO, "do");
	words["for"] = new Word(FOR, "for");
	words["case"] = new Word(CASE, "case");
	words["begin"] = new Word(BEGIN, "begin");
	words["end"] = new Word(END, "end");
	words["true"] = new Bool(true);
	words["false"] = new Bool(false);
	words["null"] = new Word(NULL_VALUE, "null");
	words["let"] = new Word(LET, "let");
	words["break"] = new Word(BREAK, "break");
	words["continue"] = new Word(CONTINUE, "continue");
	words["return"] = new Word(RETURN, "return");
	words["throw"] = new Word(THROW, "throw");
	words["try"] = new Word(TRY, "try");
	words["catch"] = new Word(CATCH, "catch");
	words["finally"] = new Word(FINALLY, "finally");
	words["switch"] = new Word(SWITCH, "switch");
	words["default"] = new Word(DEFAULT, "default");
	words["function"] = new Word(FUNCTION, "function");
	words["struct"] = new Word(STRUCT, "struct");
	words["class"] = new Word(CLASS, "class");
	words["public"] = new Word(PUBLIC, "public");
	words["private"] = new Word(PRIVATE, "private");
	words["protected"] = new Word(PROTECTED, "protected");
	words["import"] = new Word(IMPORT, "import");
}

Lexer::~Lexer(){
	words.clear();
	inf.close();
}

bool Lexer::open(string file){
	inf.open(file, ios::in);
	if (inf.is_open()){
		return true;
	}
	return false;
}

Token *Lexer::scan(){//LL(1)
	if (inf.eof()){
		return new Token(END_OF_FILE);
	}
	while (inf.read(&peek, 1)){
		column++;
		if (peek == ' ' || peek == '\t')continue;
		else if (peek == '\r')continue; // 忽略回车符
		else if (peek == '\n'){ column = 0; line++; }
		else if (peek == '/'){
			Token *t;
			if (t = skip_comment()){
				return t;
			}
			// 如果不是注释，跳出循环让后续处理处理为除法运算符
			break;
		}
		else break;
	}
	if (peek == '\''){
		return match_char();
	} else if (peek == '"'){
		return match_string();
	} else if (isalpha(peek) || peek == '_'){// 
		return match_id();//a _
	} else if (isdigit(peek)){
		return match_number();
	} else {
		// match_other现在返回Operator*，但Operator继承自Token，所以可以直接返回
		return match_other();
	}
}// a, b, c, int;

Value *Lexer::match_char(){
	char c; // '
	inf.read(&peek, 1);
	if (peek == '\\'){// '\a
		inf.read(&peek, 1);
		switch (peek){
		case 'a':c = '\a'; break;
		case 'b':c = '\b'; break;
		case 'f':c = '\f'; break;
		case 'n':c = '\n'; break;
		case 'r':c = '\r'; break;
		case 't':c = '\t'; break;
		case 'v':c = '\v'; break;
		case '\\':c = '\\'; break;
		case '\'':c = '\''; break;
		case '\"':c = '"'; break;
		case '?':c = '\?'; break;
		case '0':c = '\0'; break;
		default:
			printf("LEXICAL ERROR line[%03d]: invalid escape sequence '\\%c'\n", line, peek);
			exit(1);  // 强制退出
		}
		inf.read(&peek, 1);// 读取结束引号
	}else{
		c = peek;
		inf.read(&peek, 1);// 读取结束引号
	}
	
	// 检查是否遇到结束引号
	if (peek != '\'') {
		printf("LEXICAL ERROR line[%03d]: unterminated character literal\n", line);
		exit(1);  // 强制退出
	}
	
	return new Char(c);
}

Token *Lexer::match_id(){
	string str;
	do{
		str.push_back(peek);
		inf.read(&peek, 1);
	} while (isalnum(peek) || peek == '_');
	inf.seekg(-1, ios_base::cur);
	if (words.find(str) != words.end()){
		return words[str];
	}
	Word *w = new Word(ID, str);
	words[str] = w;
	return w;
}

Value *Lexer::match_number(){
	if (peek == '0'){
		inf.read(&peek, 1);
		if (peek == 'x'){
			return match_hex();
		} else if (isdigit(peek) && peek >= '1' && peek <= '7'){
			return match_oct();
		}
		inf.seekg(-1, ios_base::cur);
		return new Integer(0);
	}
	else{
		return match_decimal();
	}
}

Value *Lexer::match_decimal(){
	int val = 0;
	bool isFloat = false;
	double floatVal = 0.0;
	double decimalPart = 0.1;
	
	// 读取整数部分
	do{
		val = val * 10 + peek - '0';
		inf.read(&peek, 1);
	} while (isdigit(peek));
	
	// 检查是否有小数点
	if (peek == '.'){
		isFloat = true;
		floatVal = (double)val;
		inf.read(&peek, 1);
		
		// 读取小数部分
		while (isdigit(peek)){
			floatVal += (peek - '0') * decimalPart;
			decimalPart *= 0.1;
			inf.read(&peek, 1);
		}
	}
	
	inf.seekg(-1, ios_base::cur);
	
	// 根据是否为浮点数返回相应的Token
	if (isFloat){
		printf("DEBUG: Creating Double token with value %f\n", floatVal);
		return new Double(floatVal);
	} else {
		printf("DEBUG: Creating Integer token with value %d\n", val);
		return new Integer(val);
	}
}

Value *Lexer::match_hex(){
	int val = 0;
	inf.read(&peek, 1);
	do{
		if (isdigit(peek)){
			val = val * 16 + peek - '0';
		}
		else if (peek >= 'a' && peek <= 'f'){
			val = val * 16 + peek - 'a' + 10;
		}
		else if (peek >= 'A' && peek <= 'F'){
			val = val * 16 + peek - 'A' + 10;
		}
		inf.read(&peek, 1);
	} while (isxdigit(peek));
	inf.seekg(-1, ios_base::cur);
	return new Integer(val);
}

Value *Lexer::match_oct(){
	int val = 0;
	do{
		val = val * 8 + peek - '0';
		inf.read(&peek, 1);
	} while (isdigit(peek) && peek >= '0' && peek <= '7');
	inf.seekg(-1, ios_base::cur);
	return new Integer(val);
}

Operator *Lexer::match_other(){
	// 处理多字符运算符
	if (peek == '=') {
		inf.read(&peek, 1);
		if (peek == '=') {
			// ==
			inf.read(&peek, 1);
			return Operator::EQ;  // 使用静态常量
		} else {
			// =
			inf.seekg(-1, ios_base::cur);
			return new Operator(ASSIGN, "=", 2, false);  // 赋值运算符，优先级2，右结合
		}
	} else if (peek == '!') {
		inf.read(&peek, 1);
		if (peek == '=') {
			// !=
			inf.read(&peek, 1);
			return Operator::NE;  // 使用静态常量
		} else {
			// !
			inf.seekg(-1, ios_base::cur);
			return Operator::Not;  // 使用静态常量
		}
	} else if (peek == '<') {
		inf.read(&peek, 1);
		if (peek == '=') {
			// <=
			inf.read(&peek, 1);
			return Operator::LE;  // 使用静态常量
		} else {
			// <
			inf.seekg(-1, ios_base::cur);
			return Operator::LT;  // 使用静态常量
		}
	} else if (peek == '>') {
		inf.read(&peek, 1);
		if (peek == '=') {
			// >=
			inf.read(&peek, 1);
			return Operator::GE;  // 使用静态常量
		} else {
			// >
			inf.seekg(-1, ios_base::cur);
			return Operator::GT;  // 使用静态常量
		}
	} else if (peek == '&') {
		inf.read(&peek, 1);
		if (peek == '&') {
			// &&
			inf.read(&peek, 1);
			return Operator::AND;  // 使用静态常量
		} else {
			// &
			inf.seekg(-1, ios_base::cur);
			return Operator::BitAnd;  // 使用静态常量
		}
	} else if (peek == '|') {
		inf.read(&peek, 1);
		if (peek == '|') {
			// ||
			inf.read(&peek, 1);
			return Operator::OR;  // 使用静态常量
		} else {
			// |
			inf.seekg(-1, ios_base::cur);
			return Operator::BitOr;  // 使用静态常量
		}
	} else if (peek == '+') {
		inf.read(&peek, 1);
		if (peek == '+') {
			// ++
			inf.read(&peek, 1);
			return Operator::Increment;  // 使用静态常量
		} else {
			// +
			inf.seekg(-1, ios_base::cur);
			return Operator::Add;  // 使用静态常量
		}
	} else if (peek == '-') {
		inf.read(&peek, 1);
		if (peek == '-') {
			// --
			inf.read(&peek, 1);
			return Operator::Decrement;  // 使用静态常量
		} else {
			// -
			inf.seekg(-1, ios_base::cur);
			return Operator::Sub;  // 使用静态常量
		}
	} else if (peek == '*') {
		inf.read(&peek, 1);
		return Operator::Mul;  // 使用静态常量
	} else if (peek == '/') {
		inf.read(&peek, 1);
		return Operator::Div;  // 使用静态常量
	} else if (peek == '%') {
		inf.read(&peek, 1);
		return Operator::Mod;  // 使用静态常量
	} else {
		// 检查是否为可识别的字符
		if (peek >= 32 && peek <= 126) {  // 可打印ASCII字符
			// 单字符运算符
			return new Operator(peek, string(1, peek), 0, true);  // 默认优先级0，左结合
		} else if (peek == '\n' || peek == '\r') {
			// 换行符应该在scan()方法中处理，不应该在这里处理
			// 如果到达这里，说明有逻辑错误
			printf("LEXICAL ERROR line[%03d]: unexpected newline in operator\n", line);
			exit(1);
		} else {
			// 无法识别的字符
			printf("LEXICAL ERROR line[%03d]: unrecognized character '\\x%02x'\n", line, (unsigned char)peek);
			exit(1);  // 强制退出
		}
	}
}

Token *Lexer::skip_comment(){
	if (peek == '/'){
		inf.read(&peek, 1);
		if (peek == '/'){
			// 单行注释
			while (peek != '\n' && !inf.eof()){
				inf.read(&peek, 1);
			}
			return nullptr;
		}
		else if (peek == '*'){
			// 多行注释
			inf.read(&peek, 1);
			while (!inf.eof()){
				if (peek == '*'){
					inf.read(&peek, 1);
					if (peek == '/'){
						inf.read(&peek, 1);
						return nullptr;
					}
				}
				inf.read(&peek, 1);
			}
			return nullptr;
		}
		else{
			peek = '/';
			inf.seekg(-1, ios_base::cur);
			return new Token('/');
		}
	}
	return nullptr;
}

Value *Lexer::match_string(){
	string str;
	inf.read(&peek, 1); // 跳过开始的引号
	
	// 读取字符串内容，直到遇到结束引号
	while (peek != '"' && !inf.eof()) {
		if (peek == '\n') {
			printf("LEXICAL ERROR line[%03d]: unterminated string literal\n", line);
			exit(1);  // 强制退出
		}
		if (peek == '\\') {
			// 处理转义字符
			inf.read(&peek, 1);
			switch (peek) {
				case 'n': str += '\n'; break;
				case 't': str += '\t'; break;
				case 'r': str += '\r'; break;
				case '\\': str += '\\'; break;
				case '"': str += '"'; break;
				case '\'': str += '\''; break;
				case '0': str += '\0'; break;
				default: 
					// 无效转义字符，报错
					printf("LEXICAL ERROR line[%03d]: invalid escape sequence '\\%c'\n", line, peek);
					exit(1);  // 强制退出
					break;
			}
		} else {
			str += peek;
		}
		inf.read(&peek, 1);
	}
	
	// 跳过结束引号
	// inf.read(&peek, 1);
	
	// 返回字符串字面量token
	return new String(str);
}