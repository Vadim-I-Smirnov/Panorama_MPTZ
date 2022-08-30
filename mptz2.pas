program mptz2;

{$APPTYPE CONSOLE}

{$R *.res}

uses
  System.SysUtils, System.Classes;

type
  SmallInfo=record
    info:array[1..4] of cardinal;
  end;
  TMPTZSmallHdr=record
    mptz:array[1..4] of ansichar;
    hdr:cardinal;
    size:Int64;
  end;


procedure ExtractFileProp(fullfilename:string; var FilePath, FileName, Fileext:string);
var i,l,pindex, eindex:integer;
begin
   fileext:='';
   filename:=fullfilename;
   filepath:='';
   pindex:=0;
   eindex:=0;
   l:=length(fullfilename);
   for I := L downto 1 do
      if eindex=0 then begin
        if fullfilename[i]='.' then begin
          eindex:=i;
          fileext:=copy(fullfilename,eindex+1,L-(eindex));
          filename:=copy(fullfilename,1,(eindex-1));
        end else
        if (fullfilename[i]='/') or (fullfilename[i]='\') then begin
          eindex:=i;
          pindex:=i;
          filename:=copy(fullfilename,eindex+1,L-(eindex));
          filepath:=copy(fullfilename,1,pindex);
        end;
      end else
      if pindex=0 then begin
        if (fullfilename[i]='/') or (fullfilename[i]='\') then begin
          pindex:=i;
          filename:=copy(fullfilename,pindex+1,eindex-pindex-1);
          filepath:=copy(fullfilename,1,pindex);
        end;
      end else break;
end;

Function ElementStr(Text:string;Element:word;Separator:string):string;
var ElementText,StrText:string;
    Index,LenSeparator,PosSeparator:word;
begin
  Index:=0;
  LenSeparator:=Length(Separator);
  StrText:=Text+Separator;
  repeat
    PosSeparator:=Pos(Separator,StrText);
    If PosSeparator=1 then StrText:=Copy(StrText,LenSeparator+1,Length(strtext));
    If PosSeparator>1 then begin
      ElementText:=Copy(StrText,1,PosSeparator-1);
      StrText:=Copy(StrText,PosSeparator+LenSeparator,Length(strtext));
      inc(Index);
    end;
  until (PosSeparator=0) or (Index=Element);
  if PosSeparator>0 then ElementStr:=ElementText else ElementStr:=StrText;
end;


procedure LoadFrom(f:TFileStream; filename:string; size:Int64);
var i:Int64;
    f_out:TFileStream;
    fb:byte;
    fdir:string;
begin
try
  fdir:=ExtractFilePath(filename);
  if not DirectoryExists(fdir) then CreateDir(fdir);
   writeln(filename);

   f_out:=TFileStream.Create(filename, fmCreate);

   f_out.CopyFrom(f,size);

finally
   f_out.free;
end;

end;

  var
    fiext, command, filename, filepath, filen, out_dir, out_filename, temptext :string;
    keysN,i,j:integer;
    keys, paths:array of string;
    mptz_count, mptz_N:cardinal;
    mptz_mptz:array[1..4] of ansichar;
    mptz_info:array of SmallInfo;
    f,f_out:TFileStream;
    mptz_hdr, mptz_s_hdr:TMPTZSmallHdr;
    HDRList:TStringList;
begin
  try
    { TODO -oUser -cConsole Main : Insert code here }
    writeln('MPTZ unpack  v0.1 (20220728)   (c) Vadim Smirnov. (mail to: smirnov.vi@ntcexp.ru)');
    if paramcount>0 then begin
        filename:=Lowercase(ParamStr(1));
        keysN:=paramcount-1;
        SetLength(keys,keysN);

        for i := 0 to keysN-1 do
           keys[i]:=Lowercase(ParamStr(i+3));

        //Выполнение обработки
        if FileExists(filename) then begin
           ExtractFileProp(filename,filepath,filen, fiext);
           writeln(filepath);

           writeln(filen);

           out_dir:=filepath+filen;
           writeln(out_dir);
           if not DirectoryExists(out_dir) then CreateDir(out_dir);
           if DirectoryExists(out_dir) then begin
              out_dir:=out_dir+'\';
              f:=TFileStream.Create(filename,fmOpenRead);
              //Чтение заголовка
              f.read(mptz_hdr,sizeof(mptz_hdr));
              writeln('MPTZ elements: ',mptz_hdr.size);
              SetLength(paths,mptz_hdr.size);
              SetLength(mptz_info,mptz_hdr.size);
              for I := 0 to mptz_hdr.size-1 do
                 f.read(mptz_info[i],16);
              f.read(mptz_s_hdr,sizeof(mptz_s_hdr));
              write('MPTZ load MPT ...');
              LoadFrom(f,out_dir+filen+'.mpt',mptz_s_hdr.size);
              HDRList:=TStringList.Create;
              HDRList.LoadFromFile(out_dir+filen+'.mpt');
              //Чтение PATH
              j:=0;
              for I := 0 to HDRList.Count-1 do
                 if Pos('path',AnsiLowercase(HDRList.strings[i]))>0 then begin
                    if length(paths)=j then SetLength(paths,j+1);
                    paths[j]:=ElementStr(HDRList.strings[i],2,'=');
                    inc(j);
                 end;

              writeln('Ok.');

              for I := 0 to mptz_hdr.size-1 do begin
                 f.read(mptz_s_hdr,sizeof(mptz_s_hdr));
                 write('Load '+paths[i]+' ... ');
                 if Trim(paths[i])='' then paths[i]:=filen+'_'+IntToStr(i);

                 LoadFrom(f,out_dir+paths[i],mptz_s_hdr.size);
                 writeln('Ok.');
              end;

              //Завершение
              writeln('Ok.');
              HDRList.free;
              f.free;
           end else writeln('Error make directory '+out_dir+'');

        end;
    end else  writeln('     MPTZ2 filename.mptz');
  except
    on E: Exception do
      Writeln('Error '+E.ClassName, ': ', E.Message);
  end;
end.
