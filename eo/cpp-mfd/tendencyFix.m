function qManifestationInDisease = tendencyFix(qManifestationInDisease, genes, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT)

iter=1;
jter=1;

for iter=1:1:NUMBER_SYMPTOMS
	for jter=1:1:genes
		if qManifestationInDisease(iter,jter) < ZERO_FITNESS_LIMIT
			qManifestationInDisease(iter,jter) = ZERO_FITNESS_LIMIT;
		elseif qManifestationInDisease(iter,jter) > (1.0-ZERO_FITNESS_LIMIT)
			qManifestationInDisease(iter,jter) = 1.0-ZERO_FITNESS_LIMIT;
		end
	end
end

