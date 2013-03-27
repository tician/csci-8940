
function outie = fixer(innie,adj)

conflicts = zeros(1,73);

for iter=1:1:73
	if innie(iter)
		for jter=1:1:73
			if adj(iter,jter) && innie(jter)
				conflicts(iter) += 1;
			end
		end
	end
end

% biases a bit against lower index max values
while any(conflicts) > 0
	[w,iw]=max(conflicts);
	innie(iw) = 0;
	conflicts = zeros(1,73);
	for iter=1:1:73
		if innie(iter)
			for jter=1:1:73
				if adj(iter,jter) && innie(jter)
					conflicts(iter) += 1;
				end
			end
		end
	end	
end

outie = innie;

