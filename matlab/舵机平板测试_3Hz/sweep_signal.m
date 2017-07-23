for i=1:length(u)
    fprintf('%f, ',u(i,2));
    if mod(i,10)==9
        fprintf('\n');
    end
    
end