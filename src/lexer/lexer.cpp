#include "lexer/value.h"
#include "lexer/lexer.h"

namespace lexer {

// 词法分析器实现
Lexer::Lexer() : inf(nullptr) {
	look = nullptr; // 初始化当前token
	
	words["int"] = Type::Int;// new Word(INT, "int");
	words["double"] = Type::Double;
	words["if"] = new Word(IF, "if");
	words["then"] = new Word(THEN, "then");
	words["else"] = new Word(ELSE, "else");
	words["while"] = new Word(WHILE, "while");
	words["do"] = new Word(DO, "do");
	words["for"] = new Word(FOR, "for");
	words["case"] = new Word(CASE, "case");
	words["begin"] = new Word(BEGIN, "begin");
	words["end"] = new Word(END, "end");
	words["true"] = Bool::True;
	words["false"] = Bool::False;
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
	words["public"] = Visibility::Public;
	words["private"] = Visibility::Private;
	words["protected"] = Visibility::Protected;
	words["import"] = new Word(IMPORT, "import");
	words["null"] = new Word(NULL_VALUE, "null");
}

Lexer::~Lexer(){
	words.clear();
	// 不需要关闭流，因为指针指向的对象会自动管理
}

bool Lexer::from_file(const string& filepath){
	// 从文件加载
	file_inf.open(filepath, ios::in);
	if (file_inf.is_open()) {
		inf = &file_inf;
		return true;
	}
	return false;
}

bool Lexer::from_string(const string& code){
	// 从字符串加载
	str_inf.clear();
	str_inf.str(code);
	inf = &str_inf;
	return true;
}

Token *Lexer::scan(){//LL(1)
	if (!inf || inf->eof()){
		return Token::END_OF_FILE;
	}
	while (inf->read(&peek, 1)){
		column++;
		if (peek == ' ' || peek == '\t')continue;
		else if (peek == '\r')continue; // 忽略回车符
		else if (peek == '\n'){ column = 0; line++; continue; }
		else if (peek == '/' || peek == '#'){
			Token *t;
			if (t = skip_comment()){
				continue; // 跳过注释，继续处理下一个字符
			}
			// 如果不是注释，跳出循环让后续处理处理为运算符
			break;
		}
		else break;
	}
	
	#ifdef LEXER_DEBUG
	cout << "[LEXER DEBUG] scan() - peek char: '" << peek << "' (ASCII: " << (int)peek << ")" << endl;
	#endif
	
	// 检查是否到达输入末尾
	if (inf->eof()){
		return Token::END_OF_FILE;
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

Token *Lexer::match_char(){
	char c; // '
	inf->read(&peek, 1);
	if (peek == '\\'){// '\a
		inf->read(&peek, 1);
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
		inf->read(&peek, 1);// 读取结束引号
	}else{
		c = peek;
		inf->read(&peek, 1);// 读取结束引号
	}
	
	// 检查是否遇到结束引号
	if (peek != '\'') {
		printf("LEXICAL ERROR line[%03d]: unterminated character literal\n", line);
		exit(1);  // 强制退出
	}
	
	// 直接创建Char对象
	return getFactory()->getChar(c).get();
}

Token *Lexer::match_id(){
	string str;
	do{
		str.push_back(peek);
		inf->read(&peek, 1);
	} while (isalnum(peek) || peek == '_');
	inf->seekg(-1, ios_base::cur);
	// 重新读取peek字符，确保peek变量与文件指针同步
	inf->read(&peek, 1);
	inf->seekg(-1, ios_base::cur);
	
	cout << "[LEXER DEBUG] Identified identifier: '" << str << "'" << endl;
	
	if (words.find(str) != words.end()){
		return words[str];
	}
	// 使用TokenFlyweight管理Word对象
	Word *w = getFactory()->getWord(ID, str).get();
	words[str] = w;
	return w;
}

Token *Lexer::match_number(){
	if (peek == '0'){
		inf->read(&peek, 1);
		if (peek == 'x'){
			return match_hex();
		} else if (isdigit(peek) && peek >= '1' && peek <= '7'){
			return match_oct();
		}
		// 对于0.0等情况，直接处理
		if (peek == '.' || isdigit(peek)){
			// 重新设置peek为'0'，然后处理
			inf->seekg(-1, ios_base::cur);
			peek = '0';
			return match_decimal();
		}
		// 单独的0 - 使用TokenFactory管理
		inf->seekg(-1, ios_base::cur);
		peek = '0';
		// 直接创建Integer对象
		return getFactory()->getInteger(0).get();
	}
	else{
		return match_decimal();
	}
}

Token *Lexer::match_decimal(){
	int val = 0;
	bool isFloat = false;
	double floatVal = 0.0;
	double decimalPart = 0.1;
	
	// 读取整数部分
	do{
		val = val * 10 + peek - '0';
		inf->read(&peek, 1);
	} while (isdigit(peek));
	
	// 检查是否有小数点
	if (peek == '.'){
		isFloat = true;
		floatVal = (double)val;
		inf->read(&peek, 1);
		
		// 读取小数部分
		while (isdigit(peek)){
			floatVal += (peek - '0') * decimalPart;
			decimalPart *= 0.1;
			inf->read(&peek, 1);
		}
	}
	
	// 回退一个字符，让peek指向下一个要处理的字符
	inf->seekg(-1, ios_base::cur);
	
	// 根据是否为浮点数返回相应的Token
	if (isFloat){
		return getFactory()->getDouble(floatVal).get();
	} else {
		// 直接创建Integer对象
		return getFactory()->getInteger(val).get();
	}
}

Token *Lexer::match_hex(){
	int val = 0;
	inf->read(&peek, 1);
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
		inf->read(&peek, 1);
	} while (isxdigit(peek));
	inf->seekg(-1, ios_base::cur);
	// 使用享元模式管理Integer对象
	return getFactory()->getInteger(val).get();
}

Token *Lexer::match_oct(){
	int val = 0;
	do{
		val = val * 8 + peek - '0';
		inf->read(&peek, 1);
	} while (isdigit(peek) && peek >= '0' && peek <= '7');
	inf->seekg(-1, ios_base::cur);
	// 使用享元模式管理Integer对象
	return factory->getInteger(val).get();
}

Token *Lexer::match_other(){
	// 处理多字符运算符
	if (peek == '=') {
		inf->read(&peek, 1);
		if (peek == '=') {
			// ==
			return Operator::EQ;  // 使用静态常量
		} else {
			// =
			inf->seekg(-1, ios_base::cur);
			return Operator::Assign;  // 赋值运算符，优先级2，右结合
		}
	} else if (peek == '!') {
		inf->read(&peek, 1);
		if (peek == '=') {
			// !=
			return Operator::NE;  // 使用静态常量
		} else {
			// !
			inf->seekg(-1, ios_base::cur);
			return Operator::Not;  // 使用静态常量
		}
	} else if (peek == '<') {
		inf->read(&peek, 1);
		if (peek == '<') {
			// <<
			return Operator::LeftShift;  // 使用静态常量
		} else if (peek == '=') {
			// <=
			return Operator::LE;  // 使用静态常量
		} else {
			// <
			inf->seekg(-1, ios_base::cur);
			return Operator::LT;  // 使用静态常量
		}
	} else if (peek == '>') {
		inf->read(&peek, 1);
		if (peek == '>') {
			// >>
			return Operator::RightShift;  // 使用静态常量
		} else if (peek == '=') {
			// >=
			return Operator::GE;  // 使用静态常量
		} else {
			// >
			inf->seekg(-1, ios_base::cur);
			return Operator::GT;  // 使用静态常量
		}
	} else if (peek == '&') {
		inf->read(&peek, 1);
		if (peek == '&') {
			// &&
			return Operator::AND;  // 使用静态常量
		} else {
			// &
			inf->seekg(-1, ios_base::cur);
			return Operator::BitAnd;  // 使用静态常量
		}
	} else if (peek == '|') {
		inf->read(&peek, 1);
		if (peek == '|') {
			// ||
			return Operator::OR;  // 使用静态常量
		} else {
			// |
			inf->seekg(-1, ios_base::cur);
			return Operator::BitOr;  // 使用静态常量
		}
	} else if (peek == '+') {
		inf->read(&peek, 1);
		if (peek == '+') {
			// ++
			return Operator::Increment;  // 使用静态常量
		} else {
			// +
			inf->seekg(-1, ios_base::cur);
			return Operator::Add;  // 使用静态常量
		}
	} else if (peek == '-') {
		inf->read(&peek, 1);
		if (peek == '-') {
			// --
			return Operator::Decrement;  // 使用静态常量
		} else {
			// -
			inf->seekg(-1, ios_base::cur);
			return Operator::Sub;  // 使用静态常量
		}
	} else if (peek == '*') {
		inf->read(&peek, 1);  // 读取下一个字符
		return Operator::Mul;  // 使用静态常量
	} else if (peek == '/') {
		inf->read(&peek, 1);  // 读取下一个字符
		return Operator::Div;  // 使用静态常量
	} else if (peek == '%') {
		//inf->read(&peek, 1);  // 读取下一个字符
		return Operator::Mod;  // 使用静态常量
	} else if (peek == '^') {
		// inf->read(&peek, 1);  // 读取下一个字符
		return Operator::BitXor;  // 使用静态常量
	} else if (peek == '~') {
		// inf->read(&peek, 1);  // 读取下一个字符
		return Operator::BitNot;  // 使用静态常量
	} else if (peek == '.') {
		// inf->read(&peek, 1);  // 读取下一个字符
		// 不需要回退文件指针，因为peek已经指向下一个字符
		return Operator::Dot;  // 使用静态常量
	} else {
		// 其他字符返回Token类型
		if (peek > 31 && peek < 256){
			// 使用享元模式管理Token - 31到255之间的字符
			return factory->getToken(peek).get();
		}
		// 无法识别的字符
		printf("LEXICAL ERROR line[%03d]: unrecognized character '\\x%02x'\n", line, (unsigned char)peek);
		exit(1);  // 强制退出
	}
}

Token *Lexer::skip_comment(){
	if (peek == '/'){
		inf->read(&peek, 1);
		if (peek == '/'){
			// 单行注释 //
			string content = "//";
			while (peek != '\n' && !inf->eof()){
				content += peek;
				inf->read(&peek, 1);
			}
			// 如果遇到换行符，需要更新行号和列号
			if (peek == '\n') {
				column = 0;
				line++;
			}
			// 使用享元模式管理Comment对象
			static Comment* staticSingleLineComment = nullptr;
			if (!staticSingleLineComment) {
				staticSingleLineComment = new Comment(content, "//");
			}
			return staticSingleLineComment;
		}
		else if (peek == '*'){
			// 多行注释 /* */
			string content = "/*";
			inf->read(&peek, 1);
			while (!inf->eof()){
				content += peek;
				if (peek == '*'){
					inf->read(&peek, 1);
					content += peek;
					if (peek == '/'){
						inf->read(&peek, 1);
						// 使用享元模式管理Comment对象
						static Comment* staticMultiLineComment = nullptr;
						if (!staticMultiLineComment) {
							staticMultiLineComment = new Comment(content, "/*");
						}
						return staticMultiLineComment;
					}
				}
				// 更新行号和列号
				if (peek == '\n') {
					column = 0;
					line++;
				} else {
					column++;
				}
				inf->read(&peek, 1);
			}
			return new Comment(content, "/*");
		}
		else{
			// 不是注释，回退文件指针，让scan()方法处理除法运算
			inf->seekg(-1, ios_base::cur);
			peek = '/';
			return nullptr;
		}
	}
	else if (peek == '#'){
		// 单行注释 #
		string content = "#";
		inf->read(&peek, 1);
		while (peek != '\n' && !inf->eof()){
			content += peek;
			inf->read(&peek, 1);
		}
		// 如果遇到换行符，需要更新行号和列号
		if (peek == '\n') {
			column = 0;
			line++;
		}
		// 使用享元模式管理Comment对象
		static Comment* staticHashComment = nullptr;
		if (!staticHashComment) {
			staticHashComment = new Comment(content, "#");
		}
		return staticHashComment;
	}
	return nullptr;
}

Token *Lexer::match_string(){
	string str;
	inf->read(&peek, 1); // 跳过开始的引号
	
	// 读取字符串内容，直到遇到结束引号
	while (peek != '"' && !inf->eof()) {
		if (peek == '\\') {
			// 处理转义字符
			inf->read(&peek, 1);
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
		inf->read(&peek, 1);
	}
	
	// 跳过结束引号
	// inf->read(&peek, 1);
	
	// 返回字符串字面量token
	return new String(str);
}

// ==================== 从Parser移动过来的匹配方法 ====================

// 移动到下一个token
void Lexer::move() {
    look = scan();
}

// 语法分析器 - 匹配Tag预定义一个语法元素
bool Lexer::match(int Tag) {
    if (look->Tag == Tag) {
        move();
        return true;
    }
    // 保存当前token信息用于错误报告
    int currentTag = look->Tag;
    move();
    if (currentTag > 255)
        printf("SYNTAX ERROR line[%03d]: expected %d, got %d\n", line, Tag, currentTag);
    else
        printf("SYNTAX ERROR line[%03d]: expected '%c', got '%c'\n", line, (char)Tag, (char)currentTag);
    exit(1);  // 强制退出
    return false;
}

// 匹配单词（标识符或关键字）
string Lexer::matchIdentifier() {
    if (look->Tag == ID) {
        Word* word = static_cast<Word*>(look);
        move();
        return word ? word->word : "";
    }
    printf("SYNTAX ERROR line[%03d]: expected identifier, got %d\n", line, look->Tag);
    exit(1);
}

// 匹配类型
Type* Lexer::matchType() {
    if (look->Tag == ID) {
        // 用户定义类型，暂时返回Type::Int作为默认值
        // 这里可以根据需要扩展为查找用户定义的类型
        move();
        return Type::Int;
    } else if (look->Tag == STR) {
        move();
        return Type::String;
    } else if (look->Tag == NUM) {
        move();
        return Type::Int;
    } else if (look->Tag == DOUBLE) {
        move();
        return Type::Double;
    } else if (look->Tag == CHAR) {
        move();
        return Type::Char;
    } else if (look->Tag == BOOL) {
        move();
        return Type::Bool;
    }
    printf("SYNTAX ERROR line[%03d]: expected type, got %d\n", line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配指定类型的token
void Lexer::matchToken(int tag) {
    if (look->Tag == tag) {
        move();
        return;
    }
    if (look->Tag > 255) {
        printf("SYNTAX ERROR line[%03d]: expected %d, got %d\n", line, tag, look->Tag);
    } else {
        printf("SYNTAX ERROR line[%03d]: expected '%c', got '%c'\n", line, (char)tag, (char)look->Tag);
    }
    exit(1);
}

// 匹配操作符
Operator* Lexer::matchOperator() {
    // 检查是否为操作符（包括ASCII字符和枚举值）
    if (look->Tag == '+' || look->Tag == '-' || look->Tag == '*' || look->Tag == '/' ||
        look->Tag == '%' || look->Tag == '=' || look->Tag == '<' || look->Tag == '>' ||
        look->Tag == '!' || look->Tag == '&' || look->Tag == '|' || look->Tag == '^' ||
        look->Tag == '~' || look->Tag == '?' || look->Tag == ':' || look->Tag == '.' ||
        look->Tag == LE || look->Tag == GE || look->Tag == EQ_EQ || look->Tag == NE_EQ ||
        look->Tag == AND_AND || look->Tag == OR_OR || look->Tag == LEFT_SHIFT ||
        look->Tag == RIGHT_SHIFT || look->Tag == INCREMENT || look->Tag == DECREMENT ||
        look->Tag == ARROW) {
        Operator* op = static_cast<Operator*>(look);
		move();
        return op;
    }
    printf("SYNTAX ERROR line[%03d]: expected operator, got %d\n", line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配单词
Word* Lexer::matchWord() {
    if (look->Tag == ID) {
        Word* word = static_cast<Word*>(look);
        move();
        return word;
    }
    printf("SYNTAX ERROR line[%03d]: expected word, got %d\n", line, look->Tag);
    exit(1);
    return nullptr;
}



// 泛型匹配方法实现 - 返回Token类型
template<typename T>
T* Lexer::match() {
    // 根据期望的类型和当前token的Tag进行类型检查
    if constexpr (std::is_same<T, Integer>::value) {
        if (look->Tag == NUM) {
            T* result = static_cast<T*>(look);
            move();
            return result;
        }
    } else if constexpr (std::is_same<T, Double>::value) {
        if (look->Tag == REAL) {
            T* result = static_cast<T*>(look);
            move();
            return result;
        }
    } else if constexpr (std::is_same<T, Bool>::value) {
        if (look->Tag == BOOL) {
            T* result = static_cast<T*>(look);
            move();
            return result;
        }
    } else if constexpr (std::is_same<T, Char>::value) {
        if (look->Tag == CHAR) {
            T* result = static_cast<T*>(look);
            move();
            return result;
        }
    } else if constexpr (std::is_same<T, String>::value) {
        if (look->Tag == STR) {
            T* result = static_cast<T*>(look);
            move();
            return result;
        }
    }
    
    // 类型不匹配，报告错误
    printf("SYNTAX ERROR line[%03d]: expected %s, got tag %d\n", line, typeid(T).name(), look->Tag);
    exit(1);
    return nullptr;
}

// 模板特化实现 - 返回Token类型
template Integer* Lexer::match<Integer>();
template Double* Lexer::match<Double>();
template Bool* Lexer::match<Bool>();
template Char* Lexer::match<Char>();
template String* Lexer::match<String>();

} // namespace lexer
