parser grammar ChameleonRTParser;

options {
    tokenVocab = ChameleonRTLexer;
}

file: topLevelDeclaration*;

topLevelDeclaration: functionDecl
                   | structDecl
                   | varDeclStmt
                   | globalParamDecl
                   ;

functionDecl: (entryPointType | typeName) IDENTIFIER LEFT_PAREN parameterList? RIGHT_PAREN block;

structDecl: STRUCT IDENTIFIER LEFT_BRACE structMember* RIGHT_BRACE SEMICOLON;

structMember: typeName IDENTIFIER SEMICOLON;

parameterList: parameter (COMMA parameter)*;

parameter: modifier* typeName IDENTIFIER;

block: LEFT_BRACE statement* RIGHT_BRACE;

// NOTE: need to make sure vardecl type is not void
// TODO: Array type declaration
varDecl: CONST? typeName IDENTIFIER (EQUAL expr)?;

varDeclStmt: varDecl SEMICOLON;

// TODO: Array type declaration, cannot be void
globalParamDecl: IN CONST? typeName IDENTIFIER SEMICOLON;

statement: ifStmt
         | whileStmt
         | forStmt
         | returnStmt
         | block
         | varDeclStmt
         | exprStmt
         ;

ifStmt: IF LEFT_PAREN expr RIGHT_PAREN statement (ELSE statement)?;

whileStmt: WHILE LEFT_PAREN expr RIGHT_PAREN (statement | SEMICOLON);

forStmt: FOR LEFT_PAREN (varDecl | forInit)? SEMICOLON forCond? SEMICOLON forAdvance? RIGHT_PAREN (statement | SEMICOLON);

forInit: expr;

forCond: expr;

forAdvance: expr;

returnStmt: RETURN expr? SEMICOLON;

exprStmt: expr SEMICOLON;

// TODO: Missing pre/post increment and decrement
expr: (MINUS | BANG) expr                                     # Unary
    // Call followed by potential returned struct/array access
    | functionCall structArrayAccessChain?                    # Call
    | IDENTIFIER structArrayAccessChain                       # StructArray
    | expr STAR expr                                          # Mult
    | expr SLASH expr                                         # Div
    | expr (PLUS | MINUS) expr                                # AddSub
    | expr (LESS | LESS_EQUAL | GREATER | GREATER_EQUAL) expr # Comparison
    | expr (NOT_EQUAL | EQUAL_EQUAL) expr                     # Equality
    | expr BOOL_AND expr                                      # LogicAnd
    | expr BOOL_OR expr                                       # LogicOr
    | IDENTIFIER structArrayAccessChain? EQUAL expr           # Assign
    | LEFT_PAREN expr RIGHT_PAREN                             # Parens
    | (IDENTIFIER | INTEGER_LITERAL | FLOAT_LITERAL | TRUE | FALSE)  # Primary
    ;

functionCall: IDENTIFIER LEFT_PAREN arguments? RIGHT_PAREN;

structArrayAccessChain: (structAccess | arrayAccess)+;

structAccess: PERIOD IDENTIFIER;

arrayAccess: LEFT_BRACKET expr RIGHT_BRACKET;

arguments: expr (COMMA expr)*;

modifier: CONST
        | IN
        | OUT
        | IN_OUT
        ;

entryPointType: RAY_GEN
              | CLOSEST_HIT
              | ANY_HIT
              | INTERSECTION
              | MISS
              | COMPUTE
              ;

typeName: IDENTIFIER templateParameters?
        | TEXTURE templateParameters
        | RWTEXTURE templateParameters
        | BUFFER templateParameters
        | RWBUFFER templateParameters
        | ACCELERATION_STRUCTURE
        | RAY
        | BOOL
        | BOOL1
        | BOOL2
        | BOOL3
        | BOOL4
        | BOOL1X1
        | BOOL2X1
        | BOOL3X1
        | BOOL4X1
        | BOOL1X2
        | BOOL2X2
        | BOOL3X2
        | BOOL4X2
        | BOOL1X3
        | BOOL2X3
        | BOOL3X3
        | BOOL4X3
        | BOOL1X4
        | BOOL2X4
        | BOOL3X4
        | BOOL4X4
        | INT
        | INT1
        | INT2
        | INT3
        | INT4
        | INT1X1
        | INT2X1
        | INT3X1
        | INT4X1
        | INT1X2
        | INT2X2
        | INT3X2
        | INT4X2
        | INT1X3
        | INT2X3
        | INT3X3
        | INT4X3
        | INT1X4
        | INT2X4
        | INT3X4
        | INT4X4
        | UINT
        | UINT1
        | UINT2
        | UINT3
        | UINT4
        | UINT1X1
        | UINT2X1
        | UINT3X1
        | UINT4X1
        | UINT1X2
        | UINT2X2
        | UINT3X2
        | UINT4X2
        | UINT1X3
        | UINT2X3
        | UINT3X3
        | UINT4X3
        | UINT1X4
        | UINT2X4
        | UINT3X4
        | UINT4X4
        | FLOAT
        | FLOAT1
        | FLOAT2
        | FLOAT3
        | FLOAT4
        | FLOAT1X1
        | FLOAT2X1
        | FLOAT3X1
        | FLOAT4X1
        | FLOAT1X2
        | FLOAT2X2
        | FLOAT3X2
        | FLOAT4X2
        | FLOAT1X3
        | FLOAT2X3
        | FLOAT3X3
        | FLOAT4X3
        | FLOAT1X4
        | FLOAT2X4
        | FLOAT3X4
        | FLOAT4X4
        | DOUBLE
        | DOUBLE1
        | DOUBLE2
        | DOUBLE3
        | DOUBLE4
        | DOUBLE1X1
        | DOUBLE2X1
        | DOUBLE3X1
        | DOUBLE4X1
        | DOUBLE1X2
        | DOUBLE2X2
        | DOUBLE3X2
        | DOUBLE4X2
        | DOUBLE1X3
        | DOUBLE2X3
        | DOUBLE3X3
        | DOUBLE4X3
        | DOUBLE1X4
        | DOUBLE2X4
        | DOUBLE3X4
        | DOUBLE4X4
        | VOID
        ;

templateParameters: LESS typeName (COMMA typeName)* GREATER;

