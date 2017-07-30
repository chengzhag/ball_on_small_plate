clc
load e_ec_delta.mat
close all

%读取曲面图


%范围定义
errkpRange=50;
deltaErrkpRange=2;
deltakpRange=0.05;

errkiRange=50;
deltaErrkiRange=2;
deltakiRange=0.05;

errkdRange=50;
deltaErrkdRange=2;
deltakdRange=0.05;

%重设范围
ekp=ekp*(errkpRange/max(max(ekp)));
eki=eki*(errkiRange/max(max(eki)));
ekd=ekd*(errkdRange/max(max(ekd)));

eckp=eckp*(deltaErrkpRange/max(max(eckp)));
ecki=ecki*(deltaErrkiRange/max(max(ecki)));
eckd=eckd*(deltaErrkdRange/max(max(eckd)));

deltakp=deltakp*(deltakpRange/max(max(deltakp)));
deltaki=deltaki*(deltakiRange/max(max(deltaki)));
deltakd=deltakd*(deltakdRange/max(max(deltakd)));

%绘制曲面
figure
mesh(ekp,eckp,deltakp)
title('ekp,eckp,deltakp');
figure
mesh(eki,ecki,deltaki)
title('ekp,eckp,deltaki');
figure
mesh(ekd,eckd,deltakd)
title('ekp,eckp,deltakd');

%输出矩阵数据
deltakpX=ekp(1,:);
deltakiX=eki(1,:);
deltakdX=ekd(1,:);

deltakpY=eckp(:,1);
deltakpY=deltakpY';
deltakiY=ecki(:,1);
deltakiY=deltakiY';
deltakdY=eckd(:,1);
deltakdY=deltakdY';

mat2c(deltakpX,'fuzzyPIDDeltaKpX');
mat2c(deltakpY,'fuzzyPIDDeltaKpY');
mat2c(deltakp,'fuzzyPIDDeltaKpZ');

mat2c(deltakiX,'fuzzyPIDDeltaKiX');
mat2c(deltakiY,'fuzzyPIDDeltaKiY');
mat2c(deltaki,'fuzzyPIDDeltaKiZ');

mat2c(deltakdX,'fuzzyPIDDeltaKdX');
mat2c(deltakdY,'fuzzyPIDDeltaKdY');
mat2c(deltakd,'fuzzyPIDDeltaKdZ');

%将矩阵输出为c语言的数组
function mat2c(mat,name)
fprintf('const float %s[]={\n',name);
for i=1:(size(mat,1)-1)
    for j=1:size(mat,2)
        fprintf('%f, ',mat(i,j));
    end
    fprintf('\n');
end
for j=1:(size(mat,2)-1)
    fprintf('%f, ',mat(size(mat,1),j));
end
fprintf('%f\n};\n',mat(size(mat,1),size(mat,2)));

end




