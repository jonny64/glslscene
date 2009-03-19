using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace AseParse
{
    class Program
    {
        /// <summary>
        /// After this tag in ase file goes path to texture
        /// </summary>
        const string PREFIX = "*BITMAP ";
        
        static void Main(string[] args)
        {
            bool help = (args.Length < 2) || (args[0] == "/?") || 
                (args[0] == "-?") || (args[0] == "/help") || 
                (args[0] == "-help");
            if (help)
            {
                ShowHelp();
                return;
            }
            string inFilename = args[0];
            string outFilename = args[1];
            string pathToAppendToBitmaps = "";
            if (args.Length >= 3)
                pathToAppendToBitmaps = args[2];

            Parse(inFilename, outFilename, pathToAppendToBitmaps);
        }

        /// <summary>
        /// Replaces paths to textures (they are regognized by PREFIX) in specified file
        /// </summary>
        /// <param name="inFilename">input file</param>
        /// <param name="outFilename">output file</param>
        /// <param name="pathToAppednToBitmaps">path to texture is replaced by this prefix</param>
        /// <seealso cref="PREFIX"/> 
        private static void Parse(string inFilename, string outFilename, string pathToPrepend)
        {
            StreamReader inFile = new StreamReader(inFilename);
            StreamWriter outFile = new StreamWriter(outFilename);
            string line;
            while ((line = inFile.ReadLine()) != null)
            {
                if (line.Contains(PREFIX))
                {
                    int pathBeginPos = line.IndexOf('\"');
                    string prefix = line.Substring(0, pathBeginPos);
                    string path = line.Substring(pathBeginPos).Trim('\"');
                    string bitmapFilename = Path.GetFileName(path);
                    outFile.WriteLine(
                        prefix +
                        '\"' +
                        pathToPrepend +
                        bitmapFilename +
                        '\"'
                    );
                }
                else
                    outFile.WriteLine(line);
            }
            inFile.Close();
            outFile.Close();
        }

        /// <summary>
        /// Shows help
        /// </summary>
        private static void ShowHelp()
        {
            Console.WriteLine(
                "AseParser - an utility to parse ase files :)\n" +
                "usage: AseParser <inFile.ase> <outFile.ase> <bitmapPathPrefixWithTrailingSlash>"
                );
        }
    }
}
