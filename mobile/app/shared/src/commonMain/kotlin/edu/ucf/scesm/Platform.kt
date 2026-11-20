package edu.ucf.scesm

interface Platform {
    val name: String
}

expect fun getPlatform(): Platform