clear all;
j=0;
fid=fopen('Ocean_out_all.txt');
tline=fgetl(fid);
key1 = 'Mask: ';
key2 = ' hits history (number of hits before new owner requested) is: ';
while strcmp(tline, '------------------------------------------------------------------------------------')~=1
    tline=fgetl(fid);
end
while tline~=-1
j=j+1;
i=1;
hist{j} = zeros(1,10000);
tline=fgetl(fid);
while strcmp(tline, '------------------------------------------------------------------------------------')~=1
    tline=fgetl(fid);
end
    %while strcmp(tline, '------------------------------------------------------------------------------------')~=1
    display('1');
    tline=fgetl(fid);
    Index1 = strfind(tline, key1);
    Index2 = strfind(tline, key2);
    mask{j}{i}=sscanf(tline(Index1 + length(key1):end), '%g', 1);
    data{j}{i}=str2num(sscanf(tline(Index2 + length(key2):end), '%s'));
    if ~isempty(data{j}{i}) & data{j}{i}(1)>0
        hist{j}(data{j}{i}(1))=hist{j}(data{j}{i}(1))+1;
    end
 while strcmp(tline, '------------------------------------------------------------------------------------')~=1
     i=i+1;
     tline=fgetl(fid);
     if(tline==-1) 
         break;
     end
    Index1 = strfind(tline, key1);
    Index2 = strfind(tline, key2);
    mask{j}{i}=sscanf(tline(Index1 + length(key1):end), '%g', 1);
    data{j}{i}=str2num(sscanf(tline(Index2 + length(key2):end), '%s'));
    if ~isempty(data{j}{i}) & data{j}{i}(1)>0
        hist{j}(data{j}{i}(1))=hist{j}(data{j}{i}(1))+1;
    end
    %end
    %break;

 end
 figure;
 index_l{j} = find(hist{j}(:));
 index_l{j} = index_l{j}(end);
 plot(hist{j}(1:index_l{j}));
 xlabel('consecutive CPU requests','FontSize',20,'FontWeight','bold');
 ylabel('Occurrences','FontSize',20,'FontWeight','bold');
 title(['Coarseness of ',num2str(2^(j-1)),' Blocks'],'FontSize',20,'FontWeight','bold');
 set(gca,'FontWeight','bold','Fontsize',20)
     if j==6
        break;
    end
end