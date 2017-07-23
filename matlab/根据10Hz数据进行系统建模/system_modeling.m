% 用matlabc2d函数求z变换
h=tf(26.410614,[0.002989 0.055478 1 0 0])
hd=c2d(h,0.03)

%用一阶后向差分近似求z变换
syms s z T Hs;
s=(1-z^(-1))/T;
Hs=26.410614/(s^2*(1+0.055478*s+0.002989*s^2));
expand(Hs);

% 求解前馈系统的响应函数Gs
Gs=1/Hs
Gs=simplify(Gs)
Gs=collect(Gs,z)%合并同类项
Gs=vpa(Gs,6);%转化分数为小数
