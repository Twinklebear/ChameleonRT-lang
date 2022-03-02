lexer grammar ChameleonRTLexer;

// Control flow

IF: 'if';
ELSE: 'else';
FOR: 'for';
WHILE: 'while';
DO: 'do';
CONTINUE: 'continue';
BREAK: 'break';
SWITCH: 'switch';
CASE: 'case';
DEFAULT: 'default';

// Types

BOOL: 'bool';

BOOL1: 'bool1';
BOOL2: 'bool2';
BOOL3: 'bool3';
BOOL4: 'bool4';

BOOL1X1: 'bool1x1';
BOOL2X1: 'bool2x1';
BOOL3X1: 'bool3x1';
BOOL4X1: 'bool4x1';

BOOL1X2: 'bool1x2';
BOOL2X2: 'bool2x2';
BOOL3X2: 'bool3x2';
BOOL4X2: 'bool4x2';

BOOL1X3: 'bool1x3';
BOOL2X3: 'bool2x3';
BOOL3X3: 'bool3x3';
BOOL4X3: 'bool4x3';

BOOL1X4: 'bool1x4';
BOOL2X4: 'bool2x4';
BOOL3X4: 'bool3x4';
BOOL4X4: 'bool4x4';

INT: 'int';

INT1: 'int1';
INT2: 'int2';
INT3: 'int3';
INT4: 'int4';

INT1X1: 'int1x1';
INT2X1: 'int2x1';
INT3X1: 'int3x1';
INT4X1: 'int4x1';

INT1X2: 'int1x2';
INT2X2: 'int2x2';
INT3X2: 'int3x2';
INT4X2: 'int4x2';

INT1X3: 'int1x3';
INT2X3: 'int2x3';
INT3X3: 'int3x3';
INT4X3: 'int4x3';

INT1X4: 'int1x4';
INT2X4: 'int2x4';
INT3X4: 'int3x4';
INT4X4: 'int4x4';

UINT: 'uint';

UINT1: 'uint1';
UINT2: 'uint2';
UINT3: 'uint3';
UINT4: 'uint4';

UINT1X1: 'uint1x1';
UINT2X1: 'uint2x1';
UINT3X1: 'uint3x1';
UINT4X1: 'uint4x1';

UINT1X2: 'uint1x2';
UINT2X2: 'uint2x2';
UINT3X2: 'uint3x2';
UINT4X2: 'uint4x2';

UINT1X3: 'uint1x3';
UINT2X3: 'uint2x3';
UINT3X3: 'uint3x3';
UINT4X3: 'uint4x3';

UINT1X4: 'uint1x4';
UINT2X4: 'uint2x4';
UINT3X4: 'uint3x4';
UINT4X4: 'uint4x4';

FLOAT: 'float';

FLOAT1: 'float1';
FLOAT2: 'float2';
FLOAT3: 'float3';
FLOAT4: 'float4';

FLOAT1X1: 'float1x1';
FLOAT2X1: 'float2x1';
FLOAT3X1: 'float3x1';
FLOAT4X1: 'float4x1';

FLOAT1X2: 'float1x2';
FLOAT2X2: 'float2x2';
FLOAT3X2: 'float3x2';
FLOAT4X2: 'float4x2';

FLOAT1X3: 'float1x3';
FLOAT2X3: 'float2x3';
FLOAT3X3: 'float3x3';
FLOAT4X3: 'float4x3';

FLOAT1X4: 'float1x4';
FLOAT2X4: 'float2x4';
FLOAT3X4: 'float3x4';
FLOAT4X4: 'float4x4';

DOUBLE: 'double';

DOUBLE1: 'double1';
DOUBLE2: 'double2';
DOUBLE3: 'double3';
DOUBLE4: 'double4';

DOUBLE1X1: 'double1x1';
DOUBLE2X1: 'double2x1';
DOUBLE3X1: 'double3x1';
DOUBLE4X1: 'double4x1';

DOUBLE1X2: 'double1x2';
DOUBLE2X2: 'double2x2';
DOUBLE3X2: 'double3x2';
DOUBLE4X2: 'double4x2';

DOUBLE1X3: 'double1x3';
DOUBLE2X3: 'double2x3';
DOUBLE3X3: 'double3x3';
DOUBLE4X3: 'double4x3';

DOUBLE1X4: 'double1x4';
DOUBLE2X4: 'double2x4';
DOUBLE3X4: 'double3x4';
DOUBLE4X4: 'double4x4';

BUFFER: 'Buffer';
RWBUFFER: 'RWBuffer';

TEXTURE: 'Texture' [1-3] 'D';
RWTEXTURE: 'RWTexture' [1-3] 'D';

ACCELERATION_STRUCTURE: 'AccelerationStructure';
RAY: 'Ray';

VOID: 'void';

STRUCT: 'struct';

TRUE: 'true';
FALSE: 'false';

// Other keywords

RAY_GEN: 'ray_gen';
CLOSEST_HIT: 'closest_hit';
ANY_HIT: 'any_hit';
INTERSECTION: 'intersection';
MISS: 'miss';
COMPUTE: 'compute';

CONST: 'const';
OUT: 'out';
IN: 'in';
IN_OUT: 'inout';

RETURN: 'return';

// Operators

MINUS: '-';
BANG: '!';
PLUS: '+';
STAR: '*';
SLASH: '/';
PLUS_PLUS: '++';
LESS: '<';
LESS_EQUAL: '<=';
GREATER: '>';
GREATER_EQUAL: '>=';
NOT_EQUAL: '!=';
EQUAL_EQUAL: '==';
EQUAL: '=';

BOOL_AND: '&&';
BOOL_OR: '||';

SEMICOLON: ';';
COMMA: ',';
PERIOD: '.';

// Identifiers and primitives

INTEGER_LITERAL: ('0' | '-'? [1-9]) [0-9]*
               | ('0x' | '0X') [0-9a-fA-F]+
               ;

// TODO: Exponent parsing
FLOAT_LITERAL: ('0' | '-'? [1-9]) [0-9]* (PERIOD [0-9]+ | PERIOD)? [fF]?;

IDENTIFIER: [a-zA-Z_] [a-zA-Z0-9_]*;

// Other

LEFT_PAREN: '(';
RIGHT_PAREN: ')';

LEFT_BRACE: '{';
RIGHT_BRACE: '}';

LEFT_BRACKET: '[';
RIGHT_BRACKET: ']';

// TODO: It would be nice to preserve these in the translated source code
LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;
WHITESPACE: [ \t\n\r]+ -> skip;
