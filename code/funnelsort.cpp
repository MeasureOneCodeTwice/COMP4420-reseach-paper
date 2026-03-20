class Kfunnel {
    int kcubed;
    int rootk; 
    int k;
    private:
        std::vector<int> outputBuff;
        Kfunnel topFunnel;
        std::vector<Kfunnel> children;
    public:
        Kfunnel;

}

Kfunnel::setupConstants(int k) {
    this.kcubed = k * k * k;
    this.rootk = std::pow(k, 1/2);
    this.k = k;
}

Kfunnel::Kfunnel(int k, std::vector<std::vector<int>> inputBuffers, std::vector<int> outputBuffer) {
    this.setupConstants(k);

    if((this.outputBuffer = outputBuffer) == nullptr) {
        this.outputBuffer = std::vector<int>(this.kcubed);
    }

    this.children = new std::vector<Kfunnel>(this.rootk);
    for(int i = 0; i < k - this.rootk; i += this.rootk) {
        this.children.set(i, new Kfunnel(std::vector<std::vector<int>>(i, i + this.rootk - 1));
    }

    this.topFunnel = new Kfunnel(this.rootk, this.children.map(x => x.outputBuffer), outputBuffer);
}

Kfunnel::fill() {
    if(this.k <= 2) {
        /* this.outputBuffer.set(this.inputBuffer */
    } else {
        this.topFunnel.fill();
    }
}
