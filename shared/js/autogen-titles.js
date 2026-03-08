var AnimationTracker = 1;

function generateTitle(id){
    const Element = document.getElementById(id);
    var words = Element.innerHTML.split("");
    console.log(words);
    Element.innerHTML = ""
    for(var k=0;k<words.length;k++){
        Element.innerHTML += `<span style="--i:${AnimationTracker}">${words[k] == " " ? "&nbsp;" : words[k]}</span>`
        AnimationTracker ++;
    }
}

generateTitle("upper-desc");
AnimationTracker = 1;
generateTitle("center-desc");
AnimationTracker = 1;
generateTitle("lower-desc");
