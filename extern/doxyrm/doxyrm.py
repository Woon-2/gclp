# Usage example:
# python doxyrm.py -i source_with_doxygen.cpp -o source_without_doxygen.cpp

import re
import argparse as ap

doxygen_pattern = re.compile(r'\/\*\*[\s\S]*?\*\/')

def remove_doxygen_comments(input, output):
    with open(input, 'r') as file:
        icode = file.read()
    
    ocode = re.sub(doxygen_pattern, '', icode)

    with open(output, 'w') as file:
        file.write(ocode)
        
def main():
    parser = ap.ArgumentParser(description='Remove Doxygen comments from a source file.')
    parser.add_argument('-i', '--input', help='Input file name', required=True)
    parser.add_argument('-o', '--output', help='Output file name', required=True)
    args = parser.parse_args()

    ifile = args.input
    ofile = args.output

    remove_doxygen_comments(ifile, ofile)
    print(f'Doxygen comments removed and saved to {ofile}')

if __name__ == '__main__':
    main()