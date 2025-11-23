#include <Parser.hpp>
#include <Scanner.hpp>
#include <Operation.hpp>
#include <iostream>
#include <algorithm>

InternalRepresentation Parser::parse() {
    
    std::list<Operation> operations;
    int maxSR = -1;
    int error = 0;

    Token token = scanner.nextToken();    
    while (token.category != Category::CAT_EOF) {
        this->line++;
        switch (token.category) {
            case Category::CAT_MEMOP:
                try {
                    Operation op = this->finishMEMOP(static_cast<Opcode>(token.lexeme));
                    operations.push_back(op);
                    maxSR = std::max({maxSR, op.op1.SR, op.op3.SR});
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }    
                break;
            case Category::CAT_LOADI:
                try {
                    Operation op = this->finishLOADI(static_cast<Opcode>(token.lexeme));
                    operations.push_back(op);
                    maxSR = std::max({maxSR, op.op3.SR});
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_ARITHOP:
                try {
                    Operation op = this->finishARITHOP(static_cast<Opcode>(token.lexeme));
                    operations.push_back(op);
                    maxSR = std::max({maxSR, op.op1.SR, op.op2.SR, op.op3.SR});
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_OUTPUT:
                try {
                    operations.push_back(this->finishOUTPUT(static_cast<Opcode>(token.lexeme)));
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_NOP:
                try {
                    operations.push_back(this->finishNOP(static_cast<Opcode>(token.lexeme)));
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_EOL:
                break;
            default:
                this->handleInvalidToken(InvalidTokenException(token, "Operation starts with an invalid opcode."));
                error++;
        }
        token = scanner.nextToken();
    }

    if (error > 0) {
        throw ParseFailedException("Parse failed with " + std::to_string(error) + " errors.");
    }
    return {operations, maxSR};
}

Operation Parser::finishMEMOP(Opcode opcode) {
    
    Operation op;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing source register in MEMOP.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_INTO) {
        throw InvalidTokenException(token, "Missing \"=>\" in MEMOP.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing target register in MEMOP.");
    }
    op.op3.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in MEMOP.");
    }

    return op;
}

Operation Parser::finishLOADI(Opcode opcode) {
    
    Operation op;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_CONSTANT) {
        throw InvalidTokenException(token, "Missing constant in LOADI.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_INTO) {
        throw InvalidTokenException(token, "Missing \"=>\" in LOADI.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing target register in LOADI.");
    }
    op.op3.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in LOADI.");
    }

    return op;
}

Operation Parser::finishARITHOP(Opcode opcode) {
    
    Operation op;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing first source register in ARITHOP.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_COMMA) {
        throw InvalidTokenException(token, "Missing \",\" in ARITHOP.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing second source register in ARITHOP.");
    }
    op.op2.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_INTO) {
        throw InvalidTokenException(token, "Missing \"=>\" in ARITHOP.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing target register in ARITHOP.");
    }
    op.op3.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in ARITHOP.");
    }

    return op;
}

Operation Parser::finishOUTPUT(Opcode opcode) {
    
    Operation op;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_CONSTANT) {
        throw InvalidTokenException(token, "Missing constant in OUTPUT.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in OUTPUT.");
    }

    return op;
}

Operation Parser::finishNOP(Opcode opcode) {
    Operation op;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in NOP.");
    }

    return op;
}

void Parser::readToNextLine () {
    Token token = scanner.nextToken();
    while (token.category != Category::CAT_EOL) {
        if (token.category == Category::CAT_EOF) {
            throw new UnexpectedEOFException("Unexpected EOF at line " + std::to_string(this->line));
        }
        token = scanner.nextToken();
    }
}

void Parser::handleInvalidToken(const InvalidTokenException& e) {
    std::cerr << "ERROR " << this->line << ": "  << e.what() << std::endl;
    if (e.getToken().category == Category::CAT_EOF) {
        throw new UnexpectedEOFException("Unexpected EOF at line " + std::to_string(this->line));
    } else if (e.getToken().category != Category::CAT_EOL) {
        this->readToNextLine();
    }
}
