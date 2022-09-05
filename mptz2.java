import java.io.*;

    public class MPTZ2 {
        public static void ExtractFileName(String name, String[] fname)
        {
            String filepath = "";
            String filename = name;
            String fileext = "";

            int found = name.lastIndexOf("\\");
            if (found!=-1)
            {
                filepath = name.substring(0,found+1);
                filename = name.substring(found+1);
            }

            found = filename.lastIndexOf(".");
            if (found!=-1) {
                fileext = filename.substring(found + 1);
                filename = filename.substring(0, found);
            }

            fname[0]=filepath;
            fname[1]=filename;
            fname[2]=fileext;
        }

        public static void SaveToFile(FileInputStream f, String filename, long size) throws Exception
        {
            System.out.print("Create file "+filename);
            String[] fname= new String[3];

            ExtractFileName(filename, fname);
            File theDir = new File(fname[0]);
            if (!theDir.exists())
            {
                theDir.mkdirs();
            }

            //Save byte by byte
            File file = new File(filename);
            FileOutputStream outf = new FileOutputStream(file);
            byte[] allBytes = new byte[(int) size];
            int bytesRead = f.read(allBytes);
            outf.write(allBytes, 0, bytesRead);
            outf.close();
            //
            System.out.println(" Ок.");

        }
        public static long ReadLong(FileInputStream f) throws IOException
        {
            int byte8= f.read();
            int byte7= f.read();
            int byte6= f.read();
            int byte5= f.read();
            int byte4= f.read();
            int byte3= f.read();
            int byte2= f.read();
            int byte1= f.read();

            return (((long)(byte1 & 0xff) << 56) |
                    ((long)(byte2 & 0xff) << 48) |
                    ((long)(byte3 & 0xff) << 40) |
                    ((long)(byte4 & 0xff) << 32) |
                    ((long)(byte5 & 0xff) << 24) |
                    ((long)(byte6 & 0xff) << 16) |
                    ((long)(byte7 & 0xff) <<  8) |
                    ((long)(byte8 & 0xff)));
        }

        public static int ReadInt(FileInputStream f) throws IOException
        {

            int byte4= f.read();
            int byte3= f.read();
            int byte2= f.read();
            int byte1= f.read();

            return (((int)(byte1 & 0xff) << 24) |
                    ((int)(byte2 & 0xff) << 16) |
                    ((int)(byte3 & 0xff) <<  8) |
                    ((int)(byte4 & 0xff)));
        }

        public static void main(String[] args) throws Exception
        {
            System.out.println("MPTZ unpack v0.3 (20220812) smirnov.vi@ntcexp.ru ");

            if (args.length>0) {
                File file = new File(args[0]);
                if (file.exists() && !file.isDirectory()) {
                    //
                    System.out.println("Unpack file " + args[0]+" ");

                    String[] fname= new String[3];
                    String ffiledir;

                    ExtractFileName(args[0], fname);

                    if (fname[2].length()==0) {
                        ffiledir="_";
                    } else {
                        ffiledir="";
                    }

                    if (fname[0].length()>0) {
                        ffiledir=fname[0]+ffiledir+fname[1];
                    }  else {ffiledir=ffiledir+fname[1];}


                    System.out.print("Create directory "+ffiledir);
                    File theDir = new File(ffiledir);
                    if (!theDir.exists())
                    {
                        theDir.mkdirs();
                    }
                    System.out.println(" Ok.");

                    //open file
                    try(FileInputStream f = new FileInputStream(file))
                    {
                        MPTZInfo TOP_MPTZ= new MPTZInfo(0,0);
                        MPTZInfo S_MPTZ= new MPTZInfo(0,0);
                        int MPTZ=ReadInt(f);
                        TOP_MPTZ.HDR=ReadInt(f);
                        TOP_MPTZ.Size=ReadLong(f);

                        for (long i=0; i<TOP_MPTZ.Size;i++){
                            for (int j=0; j<4;j++) {
                                int Info = ReadInt(f);
                            }
                        }
                        MPTZ=ReadInt(f);
                        S_MPTZ.HDR=ReadInt(f);
                        S_MPTZ.Size=ReadLong(f);
                        //save MPT
                        SaveToFile(f,ffiledir+"\\"+fname[1]+".mpt",S_MPTZ.Size);
                        //path name array
                        String[] paths = new String[(int)TOP_MPTZ.Size];
                        for (int i=0; i<TOP_MPTZ.Size;i++){
                            paths[i]=fname[1]+String.valueOf(i);
                        }

                        //OPEN mpt FOR PATH
                        int findex=0;
                        File filempt = new File(ffiledir+"\\"+fname[1]+".mpt");
                        if (filempt.exists() && !filempt.isDirectory()) {
                            try (BufferedReader br = new BufferedReader(new FileReader(filempt))) {
                                String line;
                                while ((line = br.readLine()) != null) {
                                    //PATH
                                    int found = line.lastIndexOf("PATH");
                                    if (found!=-1)
                                    {
                                        found = line.lastIndexOf("=");
                                        if (found!=-1)
                                        {
                                            paths[findex]=line.substring(found+1);
                                            findex++;
                                        }
                                    }
                                }
                            } catch (IOException ex) {
                                System.out.println(ex.getMessage());
                            }
                        }


                        //Create all files MPTZ
                        for (int i=0; i<TOP_MPTZ.Size;i++){
                            MPTZ=ReadInt(f);
                            S_MPTZ.HDR=ReadInt(f);
                            S_MPTZ.Size=ReadLong(f);
                            //save MPT
                            SaveToFile(f,ffiledir+"\\"+paths[i],S_MPTZ.Size);
                        }
                        //close
                        f.close();
                        System.out.println("Ok.");
                    }
                    catch(IOException ex)
                    {
                        System.out.println(ex.getMessage());
                    }
                } else {
                    System.out.println("File " + args[0] + " is not exists!");
                }
            } else {
                System.out.println("mptz2 filename.mptz");
            }
        }
    }

class MPTZInfo
{
    public String MPTZ;
    public int HDR;
    public long Size;

    public MPTZInfo(int HDR, long Size)
    {
        this.MPTZ="MPTZ";
        this.HDR=HDR;
        this.Size=Size;
    }
}