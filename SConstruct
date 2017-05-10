# The MIT License (MIT)
# 
# Copyright (c) 2015 Jonathan McCluskey and William Harding
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
 
AddOption( '--enable-clang',
           dest='clang',
           action='store_true' )

env = Environment( CCFLAGS = '-Wall -Werror -g',
                   CLANG = GetOption('clang'))

if env['CLANG']:
    env.Replace( CC      = 'clang' )
    env.Replace( CXX     = 'clang++' )
    env.Append(  CCFLAGS = ' -std=c++11' )
else:
    env.Replace( CC      = 'gcc' )
    env.Replace( CXX     = 'g++' )
    env.Append(  CCFLAGS = ' -std=c++0x' )
        
# All output files will go to the build/ folder
VariantDir( 'build', 'src', duplicate=0 )

# Checks timestamp and then MD5 hashes it for diffs
Decider( 'MD5-timestamp' )

#
# LIBRARIES
#

# libio
libio = env.Library( 
    target = 'build/libio/libio', 
    source = Glob( 'build/libio/src/*.cpp' ),
    CPPPATH = ['build/libio/include']
    )

# libcrypto
libcrypto = env.Library( 
    target = 'build/libcrypto/libcrypto', 
    source = Glob( 'build/libcrypto/src/*.cpp' ),
    CPPPATH = ['build/libcrypto/include',
               'build/libio/include'],
    LIBS = ['gmp', 
            'gcrypt'],
    LIBPATH = [ '/usr/lib' ]
    )

#
# Binary Executables
#

# Update the environment to use the libraries
progEnv = env.Clone( CPPPATH = [ 'build/libcrypto/include', 
                                 'build/libio/include' ],
                     LIBS    = [ 'gmp',
                                 'libcrypto', 
                                 'libio', 
                                 'gcrypt'  ], 
                     LIBPATH = [ 'build/libcrypto', 
                                 'build/libio', 
                                 '/usr/lib' ] )

# bank executable
bankProgEnv = progEnv.Clone()
bankProgEnv.Prepend( CPPPATH = 'build/bank/include' )
bankProgEnv.Prepend( LIBS = 'boost_system' )
bankProgEnv.Prepend( LIBS = 'boost_serialization' )
bankProgEnv.Append( LIBS = 'pthread' )
bank = bankProgEnv.Program( 
    target = 'build/bank/bank', 
    source = Glob( 'build/bank/src/*.cpp' )
    )
Depends( bank, [libcrypto, libio] )

# buyer executable
buyerProgEnv = progEnv.Clone()
buyerProgEnv.Prepend( CPPPATH = 'build/buyer/include' )
buyerProgEnv.Prepend( LIBS = 'boost_serialization' )
buyerProgEnv.Prepend( LIBS = 'boost_system' )
buyerProgEnv.Append( LIBS = 'pthread' )
buyer = buyerProgEnv.Program( 
    target = 'build/buyer/buyer', 
    source = Glob( 'build/buyer/src/*.cpp' )
    )
Depends( buyer, [libcrypto, libio] )

# merchant executable
merchantProgEnv = progEnv.Clone()
merchantProgEnv.Prepend( CPPPATH = 'build/merchant/include' )
merchantProgEnv.Prepend( LIBS = 'boost_system' )
merchantProgEnv.Prepend( LIBS = 'boost_serialization' )
merchantProgEnv.Append( LIBS = 'pthread' )
merchant = merchantProgEnv.Program( 
    target = 'build/merchant/merchant', 
    source = Glob( 'build/merchant/src/*.cpp' )
    )
Depends( merchant, [libcrypto, libio] )

#
# Library Tests
#

# Update env to use boost
check_progEnv = progEnv.Clone()
check_progEnv.Prepend( LIBS = 'libboost_unit_test_framework' )
check_progEnv.Prepend( LIBS = 'boost_serialization' )
check_progEnv.Prepend( LIBS = 'gomp' )

# libcrypto test
# Build...
check_libcrypto = check_progEnv.Program( 
    target = 'build/libcrypto/check_libcrypto', 
    source = Glob( 'build/libcrypto/test/*.cpp' )
    )
Depends( check_libcrypto, libcrypto )

# And execute...
test_libcrypto = Command( target = 'build/libcrypto/test_libcrypto.out',
                source = 'build/libcrypto/check_libcrypto',
                action = ['./build/libcrypto/check_libcrypto &> $TARGET', 
                          'cat $TARGET' ] )
Depends( test_libcrypto, check_libcrypto )

# libio test
# Build...
check_libio = check_progEnv.Program( 
    target = 'build/libio/check_libio', 
    source = Glob( 'build/libio/test/*.cpp' )
    )
Depends( check_libio, libio )

# And execute...
test_libio = Command( target = 'build/libio/test_libio.out',
                      source = 'build/libio/check_libio',
                      action = ['./build/libio/check_libio &> $TARGET', 
                                'cat $TARGET' ] )
Depends( test_libio, check_libio)


#
# Binary Executables' Tests
#

# # bank test executable
# check_bank = bankProgEnv.Program( 
#     target = 'build/bank/check_bank', 
#     source = Glob( 'build/bank/test/*.cpp' )
#     )
# Depends( check_bank, bank )
# 
# # buyer test executable
# check_buyer = buyerProgEnv.Program( 
#     target = 'build/buyer/check_buyer', 
#     source = Glob( 'build/buyer/test/*.cpp' )
#     )
# Depends( check_buyer, buyer )
# 
# # merchant test executable
# check_merchant = merchantProgEnv.Program( 
#     target = 'build/merchant/check_merchant', 
#     source = Glob( 'build/merchant/test/*.cpp' )
#     )
# Depends( check_merchant, merchant )
# 

